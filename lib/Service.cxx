// woo
#include <woo/xl320/Service.h>

// std
#include <fstream>

// ---
using namespace woo::xl320;
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;

// log config
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)

/* ============================================================================
 *
 * */
std::string Service::ByteVector2HexStr(const std::vector<uint8_t>& vec, uint32_t limit)
{
    std::ostringstream s;
    s << "[" << vec.size() << "][";

    for(auto it=vec.begin() ; it!=vec.end() ; ++it)
    {
        if(it != vec.begin()) { s << ','; }
        s << "0x" << std::hex << (int)*it;

        limit--;
        if(limit <= 0) break;
    }

    s << ']';
    return s.str();
}

/* ============================================================================
 *
 * */
Service::Service(const char* dev, uint32_t baud)
    : mLog(boost::log::keywords::channel = "woo::xl320")
    , mSerialDevice(dev)
    , mSerialBaudrate(baud)
    , mReadBuffer(ReadBufferMaxSize)
    , mCommandInProcess(false)
    , mCommandTimer(mIos)
    , mCommandTimeout(mIos)
{
    mParseBuffer.reserve(ReadBufferMaxSize);




}

/* ============================================================================
 *
 * */
Service::~Service()
{
    stop();
}

/* ============================================================================
 *
 * */
void Service::start()
{
    boost::system::error_code ec;

    // log
    BOOST_LOG_SEV(mLog, info) << "start...";

    // Check if the port is already opened
    if (mPort)
    {
        BOOST_LOG_SEV(mLog, info) << "port is already opened";
        return;
    }

    // debug
    BOOST_LOG_SEV(mLog, info) << "device   (" << mSerialDevice   << ")";
    BOOST_LOG_SEV(mLog, info) << "baudrate (" << mSerialBaudrate << ")";

    // Create serial port
    mPort = SerialPortPtr(new boost::asio::serial_port(mIos));
    mPort->open(mSerialDevice, ec);
    if (ec)
    {
        BOOST_LOG_SEV(mLog, info)
            << "port openning failed... dev("
            << mSerialDevice << "), err(" << ec.message().c_str() << ")";
        return;
    }

    // option settings...
    mPort->set_option(boost::asio::serial_port_base::baud_rate(mSerialBaudrate));
    mPort->set_option(boost::asio::serial_port_base::character_size(8));
    mPort->set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    mPort->set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    mPort->set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    // Start ios run thread    
    boost::asio::io_service::work idleWork(mIos);
    mIosThread.reset( new std::thread([&]{ mIos.run(); }) );

    // Start async read loop
    prepareAsyncRead();

    // log
    BOOST_LOG_SEV(mLog, info) << "started";
}

/* ============================================================================
 *
 * */
void Service::stop()
{
    // Stop service and wait thread end
    mIos.stop();
    mIosThread->join();

    // 
    std::lock_guard<std::mutex> lock(mMutex);

    // Delete port if 
    if (mPort)
    {
        mPort->cancel();
        mPort->close();
        mPort.reset();
    }

    // Reset service
    mIos.reset();

    // log
    BOOST_LOG_SEV(mLog, info) << "stop";
}

/* ============================================================================
 *
 * */
void Service::sendPing()
{
    BOOST_LOG_SEV(mLog, info) << "ping";
    registerCommand( Command(Command::Type::ping) );
}

/* ============================================================================
 *
 * */
void Service::registerCommand(const Command& cmd)
{
    std::lock_guard<std::mutex> lock(mMutex);
 
    // Append command to queue
    mCommandQueue.push(cmd);

    // Reset timer to transfert execution in thread
    mCommandTimer.expires_from_now( boost::posix_time::seconds(0) );
    mCommandTimer.async_wait( boost::bind(&Service::sendNextCommand, this) );
}

/* ============================================================================
 *
 * */
Servo* Service::getServo(uint8_t id)
{
    for(auto& s : mServos)
    {
        if(s->getId() == id)
        {
            return s.get();
        }
    }
    std::shared_ptr<Servo> servoPtr(new Servo(id, this));
    mServos.push_back(servoPtr);
    return servoPtr.get();
}

/* ============================================================================
 *
 * */
void Service::sendNextCommand()
{
    std::lock_guard<std::mutex> lock(mMutex);

    BOOST_LOG_SEV(mLog, info) << "try to send next command";

    // No more command to send
    if( mCommandQueue.empty() )
    {
        BOOST_LOG_SEV(mLog, info) << "command queue is empty";
        return;
    }

    // A command is already running
    if( mCommandInProcess )
    {
        BOOST_LOG_SEV(mLog, info) << "a command is already running, wait...";
        return;
    }

    // Command is now running
    mCommandInProcess = true;
    int timeout = DefaultTimeout;

    // Take next comment
    mCurrentCommand = mCommandQueue.front();
    mCommandQueue.pop();

    // Reset ping result if it is an other ping command
    if(mCurrentCommand.getType() == Command::Type::ping)
    {
        timeout = PingTimeout;
        mPingResult.clear();
    }

    // Send command
    auto data = mCurrentCommand.toDataArray();
    mPort->write_some( boost::asio::buffer(data, data.size()) );
    
    // log
    BOOST_LOG_SEV(mLog, info) << "command sent: " << ByteVector2HexStr(data);

    // Start timeout timer
    mCommandTimeout.expires_from_now( boost::posix_time::milliseconds(timeout) );
    mCommandTimeout.async_wait( boost::bind(&Service::endCommand, this) );
}

/* ============================================================================
 *
 * */
void Service::prepareAsyncRead()
{
    // log
    BOOST_LOG_SEV(mLog, info) << "prepare async read";

    // Check that port is ready
    if (mPort.get() == NULL || !mPort->is_open())
    {
        BOOST_LOG_SEV(mLog, info) << "port not ready";
    }

    // Request async read
    mPort->async_read_some(
        boost::asio::buffer(mReadBuffer, ReadBufferMaxSize),
        boost::bind(
            &Service::readReceivedData,
            this,
            boost::asio::placeholders::error, 
            boost::asio::placeholders::bytes_transferred
            )
        );
}

/* ============================================================================
 *
 * */
void Service::readReceivedData(const boost::system::error_code& ec, size_t bytes_transferred)
{
    // lock data
    std::lock_guard<std::mutex> lock(mMutex);

    // log
    BOOST_LOG_SEV(mLog, info) << "read received data";

    // Check that port is ready
    if (mPort.get() == NULL || !mPort->is_open())
    {
        BOOST_LOG_SEV(mLog, info) << "    port not ready";
        return;
    }

    // Check if there are some errors
    if (ec)
    {
        BOOST_LOG_SEV(mLog, info) << "    async read error(" << ec.message().c_str() << ")";
        prepareAsyncRead();
        return;
    }

    // Debug ultra
    BOOST_LOG_SEV(mLog, info) << "    data received: " << ByteVector2HexStr(mReadBuffer, bytes_transferred);

    // Store data for later parse
    mParseBuffer.insert(
        mParseBuffer.end(),
        mReadBuffer.begin(),
        mReadBuffer.begin() + bytes_transferred);

    // Try to parse xl packet
    parsePacket();

    // Restart async read
    prepareAsyncRead();
}

/* ============================================================================
 *
 * */
void Service::parsePacket()
{
    // log
    BOOST_LOG_SEV(mLog, info) << "parse packet from buffer " << ByteVector2HexStr(mParseBuffer);

    // Check if the buffer is not empty
    if (mParseBuffer.empty())
    {
        BOOST_LOG_SEV(mLog, info) << "    parse buffer is empty";
        return;
    }

    // Initialize packet size
    uint32_t size = 1;
    uint32_t index = 0;

    // Function to safely increment size
    auto upSize = [&size, &index, this](int plus)
    {
        if( (size+plus) <= mParseBuffer.size() )
        {
            BOOST_LOG_SEV(mLog, info) << "    (" << size << "," << index << ",+" << plus << ")";
            size += plus;
            index = size-1;
            return true;
        }
        else
        {
            BOOST_LOG_SEV(mLog, info) << "    no more data to read need(" << size+plus << ") left(" << mParseBuffer.size() << ")";
            return false;
        }
    };

    // size = 1
    // index = 0
    if( (uint8_t)mParseBuffer[index] != (uint8_t)0xFF )
    {
        BOOST_LOG_SEV(mLog, info) << "    Packet byte 0 must be 0xFF";
        mParseBuffer.erase(mParseBuffer.begin(), mParseBuffer.begin()+size);
        return;
    }

    if( !upSize(1) ) { return ; }

    // size = 2
    // index = 1
    if( (uint8_t)mParseBuffer[index] != (uint8_t)0xFF )
    {
        BOOST_LOG_SEV(mLog, info) << "    Packet byte 1 must be 0xFF";
        mParseBuffer.erase(mParseBuffer.begin(), mParseBuffer.begin()+size);
        return;
    }

    if( !upSize(1) ) { return ; }

    // size = 3
    // index = 2
    if( (uint8_t)mParseBuffer[index] != (uint8_t)0xFD )
    {
        BOOST_LOG_SEV(mLog, info) << "    Packet byte 2 must be 0xFD";
        mParseBuffer.erase(mParseBuffer.begin(), mParseBuffer.begin()+size);
        return;
    }

    if( !upSize(3) ) { return ; }

    // size = 6
    // index = 5
    uint8_t lenL = mParseBuffer[index]; // 5

    if( !upSize(1) ) { return ; }
    
    // size = 7
    // index = 6
    uint8_t lenH = mParseBuffer[index]; // 6
    int len = Packet::MakeWord(lenL, lenH);

    BOOST_LOG_SEV(mLog, info) << "    packet len(" << len << ")";
    
    if(!upSize(len)) { return ; }

    BOOST_LOG_SEV(mLog, info) << "    packet complete size(" << size << ")";
    
    // Use packet
    Packet pack(mParseBuffer.data(), size);
    processPacket(pack);

    // Remove used data
    mParseBuffer.erase(mParseBuffer.begin(), mParseBuffer.begin()+size);

    // Try to parse an other packet
    parsePacket();
}

/* ============================================================================
 *
 * */
void Service::processPacket(const Packet& pack)
{
    BOOST_LOG_SEV(mLog, info) << "process packet(" << pack << ")";

    switch(mCurrentCommand.getType())
    {
        case Command::Type::none: break;
        case Command::Type::ping: processPacket_Ping(pack); break;
        case Command::Type::pull: processPacket_Pull(pack); break;
        case Command::Type::push: processPacket_Push(pack); break;
    }
}

/* ============================================================================
 *
 * */
void Service::processPacket_Ping(const Packet& pack)
{
    switch(pack.getInstruction())
    {
        case Packet::Instruction::InsPing:
        {
            BOOST_LOG_SEV(mLog, info) << "ping echo";
            break;
        }
        case Packet::Instruction::InsStatus:
        {
            // each packet received is a servo that give its ID
            uint8_t id = pack.getId();
            mPingResult.push_back( id );

            // log
            BOOST_LOG_SEV(mLog, info) << "ping new id detected: (" << (int)id << ")";

            // signal to app
            newPingIdReceived(id);
            break;
        }
        default:
        {
            BOOST_LOG_SEV(mLog, info) << "unexpected message received in ping context " << pack;
            break;
        }
    }
}

/* ============================================================================
 *
 * */
void Service::processPacket_Pull(const Packet& pack)
{
    switch(pack.getInstruction())
    {
        case Packet::Instruction::InsRead:
        {
            BOOST_LOG_SEV(mLog, info) << "    - pull command echo";
            break;
        }
        case Packet::Instruction::InsStatus:
        {
            uint8_t id = pack.getId();
            BOOST_LOG_SEV(mLog, info) << "    - pull answer for servo " << (int)id;

            Servo* servo = getServo(id);
            servo->update( (Servo::RegisterIndex)mCurrentCommand.getAddr()
                         , pack.beginReadData()
                         , pack.endReadData()
                );
            endCommand();
            break;
        }
        default:
        {
            BOOST_LOG_SEV(mLog, warning) << "    - unexpected message received in pull context " << pack;
            break;
        }
    }
}

/* ============================================================================
 *
 * */
void Service::processPacket_Push(const Packet& pack)
{

}

/* ============================================================================
 *
 * */
void Service::endCommand()
{
    mCommandInProcess = false;
    // mTimerOut.stop();

    // Signal to listeners
    commandEnded();

    // Try to send next command
    mCommandTimer.expires_from_now( boost::posix_time::seconds(0) );
    mCommandTimer.async_wait( boost::bind(&Service::sendNextCommand, this) );
}
