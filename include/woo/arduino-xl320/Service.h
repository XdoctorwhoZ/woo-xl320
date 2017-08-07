#ifndef WOO_XL320_SERVICE_H
#define WOO_XL320_SERVICE_H

#include <unistd.h>

#include <iostream>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

// ---
namespace woo { namespace xl320 {



//! 
//!
class Service
{
    //! Serial port device
    std::string mDevName;

    //! Mutex for the asio service thread
    boost::mutex mMutex;

    //! Asio service
    boost::asio::io_service mIoService;
    
    //! Serial port controller
    boost::asio::serial_port mSerialPort;

    //! Flag to telle that \r has been received
    bool rFlag;

    //! Read buffer config
    static constexpr int ReadBufRawSize = 512;
    char mReadBufRaw[ReadBufRawSize];

    //! Ready data buffer
    std::string mReadBufStr;

    boost::asio::streambuf bbbb;

    boost::thread mIoServiceThread;

public:

    Service();
    ~Service();

    void setDevName(const std::string& dev) { mDevName = dev; }

    void start();
    void stop();

    void ping();
    // getController(int )


    void asyncRead();
    void onDataReceive(const boost::system::error_code& ec, size_t bytes_transferred);
    void onDataReady(const std::string& data);


};




} // xl320
} // woo

#endif // WOO_XL320_SERVICE_H
