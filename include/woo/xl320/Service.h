#ifndef WOO_XL320_SERVICE_H
#define WOO_XL320_SERVICE_H
#pragma once

// std
#include <deque>
#include <vector>
#include <thread>
#include <sstream>

// woo
#include "Log.h"
// #include "Servo.h"

// boost
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

// ---
namespace woo { namespace xl320 {

//! Main class of xl320 servos
//! Control reception and transmission of data
class Service
{

public:

    //! For this type of data, it is easier to read data in hex
    //! This function generate a string of the vector with format [SIZE][0xFF,0xFE,0xD]
    static std::string ByteVector2HexStr(const std::vector<uint8_t>& vector, uint32_t limit = 0xFFFFFFFF);

private:

    // helpers
    typedef boost::asio::io_service BioService;
    typedef boost::shared_ptr<boost::thread> ThreadPtr;
    typedef boost::shared_ptr<boost::asio::serial_port> SerialPortPtr;

    // Configurable options for the serial device
    std::string mSerialDevice;
    uint32_t    mSerialBaudrate;
    
    //! Boost asio service object, it is required by the boost serial port
    BioService mIos;

    //! Thread in which mIos will run
    ThreadPtr mIosThread;

    //! Serial port interface
    SerialPortPtr mPort;

    //! Buffer to read incomming data from serial port
    std::vector<uint8_t> mReadBuffer;
    static constexpr int ReadBufferMaxSize = 256;

    //! Buffer that contains data that are waiting to be parsed 
    std::vector<uint8_t> mParseBuffer;

    //! mIos will run on a different thread
    //! This mutex will prevent race condition on data
    boost::mutex mMutex;





    // //! Running command indicator
    // //! pass true when command is sent
    // //! pass false when command result has been received
    // bool mIsRunning;

    // //! Commands that must be send
    // QQueue<Command> mCommandQueue;

public:

    //! Constructor
    Service(const char* dev = "/dev/ttyACM0", uint32_t baud = 115200);

    //! Destructor
    ~Service();

    //! Before using this object, it needs to be started to create serial connection
    //! and start control thread
    void start();

    //! Stop services threads
    void stop();

private:

    //! Request async read on the port
    void prepareAsyncRead();

    //! Read data received async
    void readReceivedData(const boost::system::error_code& ec, size_t bytes_transferred); 

    //! Function to register a command in the queue
    // void registerCommand(const Command& cmd);




    // Basic getters
    // bool getTestResult() const { return mResult.test; }
    // QList<int> getPingResult() const { return mResult.ping; }


    //! Take the next command in the tx queue and send it
    //! If a command is already running, a timer will call back this function later
    // void sendNextCommand();


    //! Parse data from device com
    // void parseData(const QByteArray& data);


    // //! To send test request
    // void sendTest();

    //! To send a ping
    // void sendPing();


    //! To send data through com device
    // void commandTransmissionRequested(const QByteArray& data);

    // //! Emitted when the running command has received an answers or timeout
    // //!
    // void commandFinish();

    // //! Emitted when the result of the test is ready to be read
    // //!
    // void testResultReady(bool result);

};

} // xl320
} // woo

#endif // WOO_XL320_SERVICE_H
