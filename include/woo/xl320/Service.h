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
#include <chrono>

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

struct CommandExecutionStats
{
    std::chrono::milliseconds execution_time;
};

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
    typedef boost::asio::deadline_timer         AsioTimer;
    typedef boost::asio::io_service             AsioService;
    typedef boost::asio::io_service::work       AsioServiceWork;
    typedef boost::asio::serial_port            AsioSerialPort;
    typedef std::chrono::milliseconds           Milliseconds;
    typedef std::chrono::high_resolution_clock  HrClock;
    typedef std::shared_ptr<Servo>              ServoPtr;
    typedef std::shared_ptr<std::thread>        ThreadPtr;
    typedef std::shared_ptr<AsioServiceWork>    AsioServiceWorkPtr;
    typedef std::shared_ptr<AsioSerialPort>     AsioSerialPortPtr;
    typedef std::lock_guard<std::mutex>         ScopeLock;

    // Configurable options for the serial device
    std::string mSerialDevice;
    uint32_t    mSerialBaudrate;

    //! Boost asio service, it is the main object of this class
    //! This service runs the xl320 service in a separate thread
    AsioService mIos;
    ThreadPtr   mIosThread;

    //! Serial port interface
    AsioSerialPortPtr       mPort;
    uint32_t                mPortErrors;
    static constexpr int    MaxPortErrors = 20;

    // ReadBuffer : Buffer to read incomming data from serial port
    // ParseBuffer: Buffer that contains data that are waiting to be parsed 
    std::vector<uint8_t> mReadBuffer;
    std::vector<uint8_t> mParseBuffer;
    static constexpr int ReadBufferMaxSize = 256;

    //! Running indicator
    //! pass true  when is sent
    //! pass false when result has been received
    bool                mOrderInProcess;
    bool                mCommandInProcess;
    //! Commands that must be send
    std::queue<Command> mCommandQueue;
    //! Command currently executed
    Command::Order      mOrderCurrent;
    Command             mCommandCurrent;
    //! Timer to stop command that does not end alone
    AsioTimer           mOrderTimeout;
    static constexpr int OrderTimeout = 5000;
    //! Commands are the interface of the service with user
    //! User will be on an other thread, therefore command must be protected with mutex
    std::mutex          mCommandMutex;
    //!
    HrClock::time_point mCommandStartTime;

    //! All servos connected to this service
    std::list<ServoPtr> mServos;
    std::mutex          mServosMutex;

    //! List of ids reached by ping
    std::list<uint8_t>  mPingResult;
    mutable std::mutex  mPingMutex;

public:

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

    //! Get servo controller for servo id
    Servo* getServo(uint8_t id);

    //! Return the list of id from ping
    std::list<uint8_t> getPingResult() const { ScopeLock lock(mPingMutex); return mPingResult; }

    // === SIGNALS ===

    //! Emitted when the running command has executed all its orders
    boost::signals2::signal<void(CommandExecutionStats)> commandEnded;

    //! Emitted when a new servo id is detected next to a ping
    boost::signals2::signal<void(uint8_t)> newPingIdReceived;

private:

    //! Run the command state machine to send command and orders
    void commandStateMachine();

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

    //! End the order process
    void endOrder();

};

} // xl320
} // woo

#endif // WOO_XL320_SERVICE_H
