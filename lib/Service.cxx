// woo
#include <woo/arduino-xl320/Service.h>
#include <woo/arduino-xl320/Controller.h>

// std
#include <stdexcept>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
Service::Service()
    : mSerialPort(mIoService)
    , rFlag(false)
{

}

/* ============================================================================
 *
 * */
Service::~Service()
{
    // stop();
}

/* ============================================================================
 *
 * */
void Service::start()
{
        // First lock
    boost::mutex::scoped_lock look(mMutex);

    // Try to open serial port
    boost::system::error_code ec;
    mSerialPort.open(mDevName, ec);
    if (ec) {
        std::ostringstream errMsg;
        errMsg << "Error : mSerialPort.open() failed... dev="
            << mDevName << ", err=" << ec.message().c_str();
        throw std::runtime_error( errMsg.str() );
    }

    std::cout << "Start serial port " << mDevName << std::endl;

    // Option settings...
    namespace ba = boost::asio;

    ba::serial_port_base::baud_rate     BAUD  (115200);
    ba::serial_port_base::flow_control  FLOW  ( ba::serial_port_base::flow_control::none );
    ba::serial_port_base::parity        PARITY( ba::serial_port_base::parity::none );
    ba::serial_port_base::stop_bits     STOP  ( ba::serial_port_base::stop_bits::one );

    mSerialPort.set_option( BAUD );
    mSerialPort.set_option( FLOW );
    mSerialPort.set_option( PARITY );
    mSerialPort.set_option( STOP );
    mSerialPort.set_option( ba::serial_port::character_size(8) );


    ba::serial_port_base::baud_rate  BBBBBBBBB;
    mSerialPort.get_option(BBBBBBBBB);
    std::cout << BBBBBBBBB.value() << std::endl;

    ba::serial_port_base::character_size  AAA;
    mSerialPort.get_option(AAA);
    std::cout << AAA.value() << std::endl;

    ba::serial_port_base::flow_control  FFFFF;
    mSerialPort.get_option(FFFFF);
    std::cout << FFFFF.value() << std::endl;

    ba::serial_port_base::parity  PPPPP;
    mSerialPort.get_option(PPPPP);
    std::cout << PPPPP.value() << std::endl;

    ba::serial_port_base::stop_bits  SSSS;
    mSerialPort.get_option(SSSS);
    std::cout << SSSS.value() << std::endl;


    // Start async read
    asyncRead();

    // Start service thread
    boost::thread mIoServiceThread = boost::thread(boost::bind(&ba::io_service::run, &mIoService));

    // boost::system::error_code ec;
    // mIoService.run(ec);
    // std::cout << ec.message() << std::endl;
}

/* ============================================================================
 *
 * */
void Service::stop()
{
    // First lock
    boost::mutex::scoped_lock look(mMutex);

    // Cancel async operations
    try { mSerialPort.cancel(); } catch(...) { }

    // Close port
    mSerialPort.close();

    // Stop serivce
    mIoService.stop();
    mIoService.reset();
}

/* ============================================================================
 *
 * */
void Service::ping()
{
    const char command[] = "\r\nXX+PING?\r\n";
    mSerialPort.write_some( boost::asio::buffer(command, strlen(command)) );

}

/* ============================================================================
 *
 * */
void Service::asyncRead()
{
    // Check if port is open
    if (!mSerialPort.is_open()) {
        std::cerr << "Error : Serial port is not opened" << std::endl;
        return;
    }


    std::cout << "async read start" << std::endl;


    async_read_until(
        mSerialPort,
        bbbb,
        '\n',
        boost::bind(
            &Service::onDataReceive,
            this, boost::asio::placeholders::error, 
            boost::asio::placeholders::bytes_transferred)
    );


    // Start async read
    // mSerialPort.async_read_some( 
    //     boost::asio::buffer(mReadBufRaw, ReadBufRawSize),
    //     boost::bind(
    //         &Service::onDataReceive,
    //         this, boost::asio::placeholders::error, 
    //         boost::asio::placeholders::bytes_transferred));
}

/* ============================================================================
 *
 * */
void Service::onDataReceive(const boost::system::error_code& ec, size_t bytes_transferred)
{
    // First lock
    // boost::mutex::scoped_lock look(mMutex);

    mMutex.lock();

    // Reception erro
    if (ec)
    {
        if(ec == boost::asio::error::eof)
        {
            std::cerr << "Warning : serial connection closed" << std::endl;
            // asyncRead();
        }
        else
        {
            std::cerr << "Warning : " << ec.message() << std::endl;
            asyncRead();
        }
        return;
    }

    // log
    std::cout << "Data Received : " << bytes_transferred << std::endl;


    std::string s( (std::istreambuf_iterator<char>(&bbbb)), std::istreambuf_iterator<char>() );
    onDataReady(s);

    mMutex.unlock();

    // // 
    // for (int i=0 ; i<bytes_transferred ; ++i)
    // {
    //     char c = mReadBufRaw[i];
    //     switch(c)
    //     {
    //         case '\r': {
    //             rFlag = true;
    //         }
    //         break;

    //         case '\n': {
    //             if(rFlag)
    //             {
    //                 this->onDataReady(mReadBufStr);
    //                 mReadBufStr.clear();
    //             }
    //         }
    //         break;
        
    //         default:
    //         {   
    //             mReadBufStr += c;
    //             rFlag = false;
    //         }
    //     }
    // }


    // std::cout << data << std::endl;

    //
    asyncRead();
}

/* ============================================================================
 *
 * */
void Service::onDataReady(const std::string& data)
{
    std::cout << data << std::endl;
}
