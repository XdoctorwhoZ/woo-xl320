#ifndef WOO_XL320_SERVICE_H
#define WOO_XL320_SERVICE_H
#pragma once

// std
#include <list>
#include <deque>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <memory>
#include <sstream>

// woo
#include "Servo.h"

// boost
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
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
    typedef boost::asio::deadline_timer BdTimer;
    typedef std::shared_ptr<std::thread> ThreadPtr;
    typedef std::shared_ptr<boost::asio::serial_port> SerialPortPtr;

    //! Timeout values defintion in ms
    static constexpr int PingTimeout = 5000;
    static constexpr int DefaultTimeout = 5000;

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
    std::mutex mMutex;

    //! Running command indicator
    //! pass true when command is sent
    //! pass false when command result has been received
    bool mCommandInProcess;

    //! Timer to program next command sending
    BdTimer mCommandTimer;
    BdTimer mCommandTimeout;

    //! Command currently executed
    Command mCurrentCommand;

    //! Commands that must be send
    std::queue<Command> mCommandQueue;

    //! List of ids reached by ping
    std::list<uint8_t> mPingResult;

    //! All servos connected to this service
    std::list<std::shared_ptr<Servo>> mServos;

    //!
    uint32_t mSerialErrors;
    static constexpr int MaxSerialErrors = 20;

public:

    // === SIGNALS ===

    //! Emitted when the running command has received an answers or timeout
    boost::signals2::signal<void(void)> commandEnded;

    //! Emitted when a new servo id is detected next to a ping
    boost::signals2::signal<void(uint8_t)> newPingIdReceived;

    // === SIGNALS ===

    //! Constructor
    Service(const char* dev = "/dev/ttyACM0", uint32_t baud = 115200);

    //! Destructor
    ~Service();

    // Basic setters
    void setSerialDevice(const std::string& dev) { mSerialDevice = dev; }
    void setSerialBaudrate(uint32_t baud) { mSerialBaudrate = baud; }

    //! Before using this object, it needs to be started to create serial connection
    //! and start control thread
    void start();

    //! Stop services threads
    void stop();

    //! To send a ping and get available servo id connected
    void sendPing();

    //! Function to register a command in the queue
    void registerCommand(const Command& cmd);

    //! Return the list of id from ping
    std::list<uint8_t> getPingResult() const { return mPingResult; }

    //! Get servo controller for servo id
    Servo* getServo(uint8_t id);

private:

    //! Take the next command in the tx queue and send it
    //! If a command is already running, a timer will call back this function later
    void sendNextCommand();

    //! Request async read on the port
    void prepareAsyncRead();

    //! Read data received async
    void readReceivedData(const boost::system::error_code& ec, size_t bytes_transferred); 

    //! Parse packet contained in rx buffer
    void parsePacket();

    //! Use packet information to update service
    void processPacket(const Packet& pack);
    void processPacket_Ping(const Packet& pack);
    void processPacket_Pull(const Packet& pack);
    void processPacket_Push(const Packet& pack);

    //! End the command process
    void endCommand();

};

} // xl320
} // woo

#endif // WOO_XL320_SERVICE_H
