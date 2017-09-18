// woo
#include <woo/xl320/Service.h>

// std
#include <fstream>
#include <stdexcept>
    
// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
std::string Service::ByteVector2HexStr(const std::vector<uint8_t>& vec, uint32_t limit)
{
    std::ostringstream s;
    s << "[" << vec.size() << "]{";

    for(auto it=vec.begin() ; it!=vec.end() ; ++it)
    {
        if(it != vec.begin()) { s << ','; }
        s << "0x" << std::hex << (int)*it;

        limit--;
        if(limit <= 0) break;
    }

    s << '}';
    return s.str();
}

/* ============================================================================
 *
 * */
Service::Service(const char* dev, uint32_t baud)
    : mIosThread(0)
    , mPort(0)
    , mPortErrors(0)
    , mSerialDevice(dev)
    , mSerialBaudrate(baud)
    , mReadBuffer(ReadBufferMaxSize)
    , mOrderInProcess(false)
    , mCommandInProcess(false)
    , mOrderTimeout(mIos)
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

    log() << "+ Service::start()...";

    // Check if the port is already opened
    if (mPort)
    {
        log() << "    ! Port is already opened";
        return;
    }

    log() << "    - Device   (" << mSerialDevice   << ")";
    log() << "    - Baudrate (" << mSerialBaudrate << ")";

    // Create serial port
    mPort.reset(new AsioSerialPort(mIos));
    mPort->open(mSerialDevice, ec);
    if (ec)
    {
        log() << "    ! Port openning failed... dev("
              << mSerialDevice << "), err(" << ec.message().c_str() << ")";
        throw std::runtime_error("Unable to open serial port");
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
    log() << "+ Service::start()...OK";
}

/* ============================================================================
 *
 * */
void Service::stop()
{
    // log
    log() << "+ Service::stop()...";

    // Stop service and wait thread end
    mIos.stop();
    if (mIosThread) mIosThread->join();

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
    log() << "+ Service::stop()...OK";
}

/* ============================================================================
 *
 * */
void Service::sendPing()
{
    // log
    log() << "+ Service::sendPing()";

    // Prepare command with only ping order then register it
    registerCommand( Command() << Command::Order(Command::Type::ping) );
}

/* ============================================================================
 *
 * */
void Service::registerCommand(const Command& cmd)
{
    // log
    log() << "+ Service::registerCommand()";

    // Lock
    ScopeLock lock(mCommandMutex);
 
    // Append command to queue
    mCommandQueue.push(cmd);

    // Post action to execute the next command
    mIos.post(boost::bind(&Service::commandStateMachine, this));
}

/* ============================================================================
 *
 * */
Servo* Service::getServo(uint8_t id)
{
    //log
    log() << "+ Service::getServo(" << (int)id << ")";

    // lock
    ScopeLock lock(mServosMutex);

    // Check that servo is not already created for this id
    for(auto& s : mServos)
    {
        if(s->getId() == id)
        {
            log() << "    - Servo already created";
            return s.get();
        }
    }

    // Create a new servo for this id and return it
    log() << "    - Create servo";
    std::shared_ptr<Servo> servoPtr(new Servo(id, this));
    mServos.push_back(servoPtr);
    return servoPtr.get();
}

/* ============================================================================
 *
 * */
void Service::commandStateMachine()
{
    // log
    log() << "+ Service::commandStateMachine()";

    // lock
    ScopeLock lock(mCommandMutex);

    // First check if a command is already running
    if( !mCommandInProcess )
    {
        // log
        log() << "    - Try to get the next command";

        // No more command to send
        if( mCommandQueue.empty() )
        {
            log() << "    - Command queue is empty";
            return;
        }

        // Take next comment
        // Command is now running
        mCommandStartTime = HrClock::now();
        mCommandInProcess = true;
        mCommandCurrent = mCommandQueue.front();
        mCommandQueue.pop();

        // log
        log() << "    - New command selected";

        // Try to send next command
        mIos.post(boost::bind(&Service::commandStateMachine, this));
    }
    else
    {
        // Check if order is already running
        if(mOrderInProcess)
        {
            log() << "    - Order already running, wait...";
            return;
        }

        // Check if the command has no more order to execute
        if(mCommandCurrent.isOver())
        {
            // log
            log() << "    - Command is over";

            // Reset flag
            mCommandInProcess = false;

            CommandExecutionStats stats;
            stats.execution_time = std::chrono::duration_cast<Milliseconds>(HrClock::now() - mCommandStartTime);

            // Signal to listeners
            commandEnded(stats);

            // Try to send next command
            mIos.post(boost::bind(&Service::commandStateMachine, this));
            return;
        }

        // Extract next order
        mOrderInProcess = true;
        mOrderCurrent = mCommandCurrent.extract();

        // Reset ping result if it is an other ping command
        if(mOrderCurrent.type == Command::Type::ping)
        {
            mPingMutex.lock();
            mPingResult.clear();
            mPingMutex.unlock();
        }

        // Get packet from the order
        auto data = mOrderCurrent.toByteArray();

        // log
        log() << "    - Command sent: " << ByteVector2HexStr(data);

        // Send command packet
        mPort->write_some( boost::asio::buffer(data, data.size()) );

        // Start timeout timer
        mOrderTimeout.expires_from_now( boost::posix_time::milliseconds(OrderTimeout) );
        mOrderTimeout.async_wait( boost::bind(&Service::endOrder, this) );
    }
}

/* ============================================================================
 *
 * */
void Service::prepareAsyncRead()
{
    // log
    log() << "+ Service::prepareAsyncRead()";

    // Check that port is ready
    if (mPort.get() == NULL || !mPort->is_open())
    {
        log() << "    - Port not ready";
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
    // log
    log() << "+ Service::readReceivedData(" << bytes_transferred << ")";

    // lock data
    ScopeLock lock(mCommandMutex);

    // Check that port is ready
    if (mPort.get() == NULL || !mPort->is_open())
    {
        log() << "    - Port not ready";
        return;
    }

    // Check if there are some errors
    if (ec)
    {
        log() << "    - Async read error(" << ec.message().c_str() << ")";
        mPortErrors++;
        if (mPortErrors >= MaxPortErrors)
        {
            std::ostringstream err;
            err << "Number of errors max reached (last:" << ec.message() << ")";
            throw std::runtime_error(err.str());
        }
        prepareAsyncRead();
        return;
    }

    // log
    log() << "    - Data received: " << ByteVector2HexStr(mReadBuffer, bytes_transferred);

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
    log() << "+ Service::parsePacket(" << ByteVector2HexStr(mParseBuffer) << ")";

    // Check if the buffer is not empty
    if (mParseBuffer.empty())
    {
        log() << "    - Parse buffer is empty";
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
            log() << "    - (" << size << "," << index << ",+" << plus << ")";
            size += plus;
            index = size-1;
            return true;
        }
        else
        {
            log() << "    - No more data to read need(" << size+plus << ") left(" << mParseBuffer.size() << ")";
            return false;
        }
    };

    // size = 1
    // index = 0
    if( (uint8_t)mParseBuffer[index] != (uint8_t)0xFF )
    {
        log() << "    - Packet byte 0 must be 0xFF";
        mParseBuffer.erase(mParseBuffer.begin(), mParseBuffer.begin()+size);
        return;
    }

    if( !upSize(1) ) { return ; }

    // size = 2
    // index = 1
    if( (uint8_t)mParseBuffer[index] != (uint8_t)0xFF )
    {
        log() << "    - Packet byte 1 must be 0xFF";
        mParseBuffer.erase(mParseBuffer.begin(), mParseBuffer.begin()+size);
        return;
    }

    if( !upSize(1) ) { return ; }

    // size = 3
    // index = 2
    if( (uint8_t)mParseBuffer[index] != (uint8_t)0xFD )
    {
        log() << "    - Packet byte 2 must be 0xFD";
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

    log() << "    - Packet len(" << len << ")";
    
    if(!upSize(len)) { return ; }

    log() << "    - Packet complete size(" << size << ")";
    
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
    log() << "+ Service::processPacket(" << pack << ")";

    switch(mOrderCurrent.type)
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
            log() << "ping echo";
            break;
        }
        case Packet::Instruction::InsStatus:
        {
            // each packet received is a servo that give its ID
            uint8_t id = pack.getId();

            // append this id in the result vector
            mPingMutex.lock();
            mPingResult.push_back( id );
            mPingMutex.unlock();

            // log
            log() << "    - Ping new id detected: (" << (int)id << ")";

            // signal to app
            newPingIdReceived(id);
            break;
        }
        default:
        {
            log() << "    - Unexpected message received in ping context " << pack;
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
            log() << "    - Pull command echo";
            break;
        }
        case Packet::Instruction::InsStatus:
        {
            // Get id and log
            uint8_t id = pack.getId();
            log() << "    - Pull answer for servo " << (int)id;

            // Update servo data
            Servo* servo = getServo(id);
            Servo::RegisterIndex index = Servo::RegisterAddr2RegisterIndex( mOrderCurrent.addr );
            servo->update( index, pack.getReadValue() );

            // Terminate order normally
            endOrder();
            break;
        }
        default:
        {
            log() << "    - Unexpected message received in pull context " << pack;
            break;
        }
    }
}

/* ============================================================================
 *
 * */
void Service::processPacket_Push(const Packet& pack)
{
    switch(pack.getInstruction())
    {
        case Packet::Instruction::InsWrite:
        {
            log() << "    - Push command echo";
            break;
        }
        case Packet::Instruction::InsStatus:
        {
            log() << "    - Push answer";
            // Terminate order normally
            endOrder();
            break;
        }
        default:
        {
            log() << "    - Unexpected message received in push context " << pack;
            break;
        }
    }
}

/* ============================================================================
 *
 * */
void Service::endOrder()
{
    // Set flags
    mOrderInProcess = false;
    mOrderTimeout.cancel();

    // Try to send next command
    mIos.post(boost::bind(&Service::commandStateMachine, this));
}
