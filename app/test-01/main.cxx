// Qt
// #include <QCoreApplication>

#include <woo/xl320/Service.h>
//
// #include "Test01.h"

// std
#include <iostream>

// ---
using namespace std;




// #include <boost/bind.hpp>
// #include <boost/thread.hpp>
// #include <boost/asio.hpp>
// #include <boost/asio/serial_port.hpp>
// #include <boost/system/error_code.hpp>
// #include <boost/system/system_error.hpp>


// //! 
// //!
// class Service
// {


//     typedef boost::asio::io_service BioService;
//     typedef boost::shared_ptr<boost::asio::serial_port> SerialPortPtr;
    

//     BioService mIos;

//     SerialPortPtr mPort;
//     boost::mutex mMutex;


//     char read_buf_raw_[SERIAL_PORT_READ_BUF_SIZE];
//     std::string read_buf_str_;

//     char end_of_line_char_;

// public:

//     Service() : end_of_line_char_('\n')
//     {}

//     bool start(const char *com_port_name, int baud_rate)
//     {
//         // std::thread::id this_id = std::this_thread::get_id();

//         boost::system::error_code ec;

//         std::cout << "start" << std::endl;

//         if (mPort) {
//             std::cout << "error : port is already opened..." << std::endl;
//             return false;
//         }

//         mPort = SerialPortPtr(new boost::asio::serial_port(mIoService));
//         mPort->open(com_port_name, ec);
//         if (ec) {
//             std::cout << "error : port_->open() failed...com_port_name="
//                 << com_port_name << ", e=" << ec.message().c_str() << std::endl; 
//             return false;
//         }

//         // option settings...
//         mPort->set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
//         mPort->set_option(boost::asio::serial_port_base::character_size(8));
//         mPort->set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
//         mPort->set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
//         mPort->set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

//         boost::thread t(boost::bind(&boost::asio::io_service::run, &mIoService));

//         async_read_some_();

//         return true;
//     }

//     void async_read_some_()
//     {
//         if (mPort.get() == NULL || !mPort->is_open()) return;

//         mPort->async_read_some( 
//             boost::asio::buffer(read_buf_raw_, SERIAL_PORT_READ_BUF_SIZE),
//             boost::bind(
//                 &Service::on_receive_,
//                 this, boost::asio::placeholders::error, 
//                 boost::asio::placeholders::bytes_transferred));
//     }

//     void on_receive_(const boost::system::error_code& ec, size_t bytes_transferred)
//     {
//         boost::mutex::scoped_lock look(mMutex);

//         if (mPort.get() == NULL || !mPort->is_open()) return;
//         if (ec) {
//             async_read_some_();
//             return;
//         }

//         for (unsigned int i = 0; i < bytes_transferred; ++i) {
//             char c = read_buf_raw_[i];
//             if (c == end_of_line_char_) {
//                 this->on_receive_(read_buf_str_);
//                 read_buf_str_.clear();
//             }
//             else {
//                 read_buf_str_ += c;
//             }
//         }

//         async_read_some_();
//     }

//     void on_receive_(const std::string &data)
//     {
//         // std::cout << "SerialPort::on_receive_() : " << data << std::endl;
//         BOOST_LOG_TRIVIAL(info) << data;
//     }

// };



//! Main enter point
//!
int main(int argc, char *argv[])
{
    // Check if device is provided
    if( argc < 2 ) {
        cerr << "usage: test-01 /dev/tty" << endl;
        return 1;
    }

    // Start qt core application
    // QCoreApplication a(argc, argv);


    // woo::arduino_xl320::Servo s;

    // // Start test
    // Test01 testObj;
    // if( testObj.start(argv[1]) ) {
    //     return 1;   
    // }

    // boost::log::core::get()->set_filter
    // (
    //     boost::log::trivial::severity >= boost::log::trivial::info
    // );

    // BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
    // BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
    // BOOST_LOG_TRIVIAL(info) << "An informational severity message";
    // BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
    // BOOST_LOG_TRIVIAL(error) << "An error severity message";
    // BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

    woo::xl320::Service ser(argv[1], 115200);
    ser.start();


    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    ser.sendPing();
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100000));

    return 0;
}

