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
    auto work = boost::asio::io_service::work(mIos);
    // Start ios run thread
    boost::thread t(boost::bind(&boost::asio::io_service::run, &mIos));

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
    boost::mutex::scoped_lock look(mMutex);
    if (mPort)
    {
        mPort->cancel();
        mPort->close();
        mPort.reset();
    }
    mIos.stop();
    mIos.reset();

    // log
    LOG_INFO << "stop";
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
        LOG_WARNING << "port not ready";
        return;
    }

    // Check if there are some errors
    if (ec)
    {
        LOG_WARNING << "async read error(" << ec.message().c_str() << ")";
        prepareAsyncRead();
        return;
    }

    LOG_TRACE << bytes_transferred;
    LOG_TRACE << ByteVector2HexStr(mReadBuffer, bytes_transferred);

    // mParseBuffer.insert(mParseBuffer.end(), mReadBuffer.begin(), mReadBuffer.end());
    // mReadBuffer.clear();   

    // Restart async read
    prepareAsyncRead();
}

