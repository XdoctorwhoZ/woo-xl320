// woo
#include <woo/xl320/Service.h>

// ---
using namespace woo::xl320;

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
    : mSerialDevice(dev)
    , mSerialBaudrate(baud)
    , mReadBuffer(ReadBufferMaxSize)
    , mCommandInProcess(false)
    , mCommandTimer(mIos)
    , mCommandTimeout(mIos)
    // , mIdleWork(mIos)
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
    LOG_INFO << "start...";

    // Check if the port is already opened
    if (mPort)
    {
        LOG_WARNING << "port is already opened";
        return;
    }

    // debug
    LOG_DEBUG << "device   (" << mSerialDevice   << ")";
    LOG_DEBUG << "baudrate (" << mSerialBaudrate << ")";

    // Create serial port
    mPort = SerialPortPtr(new boost::asio::serial_port(mIos));
    mPort->open(mSerialDevice, ec);
    if (ec)
    {
        LOG_ERROR
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

    // You need to use boost::asio::io_service::work.
    // Think of it as a dummy work item, so io_service::run never runs out of work, and thus doesn't return immediately.
    // auto wk = 
    boost::asio::io_service::work ww(mIos);
    // Start ios run thread
    
    mIosThread.reset( new boost::thread(boost::bind(&boost::asio::io_service::run, &mIos)) );
    // mIos.run();


    // Start async read loop
    prepareAsyncRead();


    // log
    LOG_INFO << "started";
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
    boost::mutex::scoped_lock look(mMutex);

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
    LOG_INFO << "stop";
}

/* ============================================================================
 *
 * */
void Service::sendPing()
{
    LOG_INFO << "ping";
    registerCommand( Command(Command::Type::ping) );
}

/* ============================================================================
 *
 * */
void Service::registerCommand(const Command& cmd)
{
    boost::mutex::scoped_lock look(mMutex);
 
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
    boost::mutex::scoped_lock look(mMutex);

    LOG_TRACE << "try to send next command";

    // No more command to send
    if( mCommandQueue.empty() )
    {
        LOG_TRACE << "command queue is empty";
        return;
    }

    // A command is already running
    if( mCommandInProcess )
    {
        LOG_TRACE << "a command is already running, wait...";
        return;
    }

    // Command is now running
    mCommandInProcess = true;
    // int timeout = DefaultTimeout;

    // Take next comment
    mCurrentCommand = mCommandQueue.front();
    mCommandQueue.pop();

    // Reset ping result if it is an other ping command
    if(mCurrentCommand.getType() == Command::Type::ping)
    {
        // timeout = PingTimeout;
        mPingResult.clear();
    }

    // Send command
    auto data = mCurrentCommand.toDataArray();
    mPort->write_some( boost::asio::buffer(data, data.size()) );
    
    // log
    LOG_TRACE << "command sent: " << ByteVector2HexStr(data);


    // // Start timeout timer
    // mTimerOut.start(timeout);
}

/* ============================================================================
 *
 * */
void Service::prepareAsyncRead()
{
    // log
    LOG_TRACE << "prepare async read";

    // Check that port is ready
    if (mPort.get() == NULL || !mPort->is_open())
    {
        LOG_WARNING << "port not ready";
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
    boost::mutex::scoped_lock look(mMutex);

    // log
    LOG_TRACE << "read received data";

    // Check that port is ready
    if (mPort.get() == NULL || !mPort->is_open())
    {
        LOG_WARNING << "    port not ready";
        return;
    }

    // Check if there are some errors
    if (ec)
    {
        LOG_WARNING << "    async read error(" << ec.message().c_str() << ")";
        prepareAsyncRead();
        return;
    }

    // Debug ultra
    LOG_TRACE << "    data received: " << ByteVector2HexStr(mReadBuffer, bytes_transferred);

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
    LOG_TRACE << "parse packet from buffer " << ByteVector2HexStr(mParseBuffer);

    // Check if the buffer is not empty
    if (mParseBuffer.empty())
    {
        LOG_TRACE << "    parse buffer is empty";
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
            LOG_TRACE << "    (" << size << "," << index << ",+" << plus << ")";
            size += plus;
            index = size-1;
            return true;
        }
        else
        {
            LOG_TRACE << "    no more data to read need(" << size+plus << ") left(" << mParseBuffer.size() << ")";
            return false;
        }
    };

    // size = 1
    // index = 0
    if( (uint8_t)mParseBuffer[index] != (uint8_t)0xFF )
    {
        LOG_TRACE << "    Packet byte 0 must be 0xFF";
        mParseBuffer.erase(mParseBuffer.begin(), mParseBuffer.begin()+size);
        return;
    }

    if( !upSize(1) ) { return ; }

    // size = 2
    // index = 1
    if( (uint8_t)mParseBuffer[index] != (uint8_t)0xFF )
    {
        LOG_TRACE << "    Packet byte 1 must be 0xFF";
        mParseBuffer.erase(mParseBuffer.begin(), mParseBuffer.begin()+size);
        return;
    }

    if( !upSize(1) ) { return ; }

    // size = 3
    // index = 2
    if( (uint8_t)mParseBuffer[index] != (uint8_t)0xFD )
    {
        LOG_TRACE << "    Packet byte 2 must be 0xFD";
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

    LOG_TRACE << "    packet len(" << len << ")";
    
    if(!upSize(len)) { return ; }

    LOG_TRACE << "    packet complete size(" << size << ")";
    
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
    LOG_TRACE << "process packet(" << pack << ")";

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
            LOG_TRACE << "ping echo";
            break;
        }
        case Packet::Instruction::InsStatus:
        {
            // each packet received is a servo that give its ID
            uint8_t id = pack.getId();
            mPingResult.push_back( id );

            // log
            LOG_TRACE << "ping new id detected: (" << (int)id << ")";

            // signal to app
            newPingIdReceived(id);
            break;
        }
        default:
        {
            LOG_WARNING << "unexpected message received in ping context " << pack;
            break;
        }
    }
}

/* ============================================================================
 *
 * */
void Service::processPacket_Pull(const Packet& pack)
{
    // switch(pack.getInstruction())
    // {
    //     case Packet::Instruction::InsRead:
    //     {
    //         #ifdef DEBUG_PLUS
    //         qDebug() << "      - Read echo";
    //         #endif
    //         break;
    //     }
    //     case Packet::Instruction::InsStatus:
    //     {
    //         uint8_t id = pack.getId();
    //         Servo* servo = getServo(id);
    //         servo->update((Servo::RegisterIndex)mCurrentCommand.getAddr(), pack.getReadData());
    //         endCommand();
    //         break;
    //     }
    //     default:
    //     {
    //         qWarning("      - Unexpected message received in ping context %s", pack.toString().toStdString().c_str());
    //         break;
    //     }
    // }
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
    // mIsRunning = false;
    // mTimerOut.stop();

    // // Signal to listeners
    // emit commandEnded();

    // // Try to send next command
    // QTimer::singleShot(0, this, SLOT(sendNextCommand()) );
}
