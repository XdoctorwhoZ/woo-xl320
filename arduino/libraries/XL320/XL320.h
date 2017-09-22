#ifndef XL320_HPP
#define XL320_HPP
#pragma once

// Arduino
#include <stdint.h>
#include <Arduino.h>

// ---
namespace xl320 {

//! Class to build and parse data packets
class Packet
{
    // Constant header bytes
    static constexpr uint8_t Header0 = 0xFF;
    static constexpr uint8_t Header1 = 0xFF;
    static constexpr uint8_t Header2 = 0xFD;
    static constexpr uint8_t Reserve = 0x00;

    // Available instruction
    static constexpr uint8_t InstructionPing            = 0x01 ; // Instruction that checks whether the Packet has arrived to a device with the same ID as Packet ID
    static constexpr uint8_t InstructionRead            = 0x02 ; // Instruction to read data from the Device
    static constexpr uint8_t InstructionWrite           = 0x03 ; // Instruction to write data on the Device
    static constexpr uint8_t InstructionRegWrite        = 0x04 ; // Instruction that registers the Instruction Packet to a standby status; Packet is later executed through the Action command
    static constexpr uint8_t InstructionAction          = 0x05 ; // Instruction that executes the Packet that was registered beforehand using Reg Write
    static constexpr uint8_t InstructionFactoryReset    = 0x06 ; // Instruction that resets the Control Table to its initial factory default settings
    static constexpr uint8_t InstructionReboot          = 0x08 ; // Instruction to reboot the Device
    static constexpr uint8_t InstructionStatus          = 0x55 ; // Return Instruction for the Instruction Packet
    static constexpr uint8_t InstructionSyncRead        = 0x82 ; // (130) For multiple devices, Instruction to read data from the same Address with the same length at once
    static constexpr uint8_t InstructionSyncWrite       = 0x83 ; // (131) For multiple devices, Instruction to write data on the same Address with the same length at once

    // Id to broadcast to all servos
    static constexpr uint8_t PacketError = 42;

    // Id to broadcast to all servos
    static constexpr uint8_t BroadcastId = 0xFE;

    //! To update CRC of packets
    static uint16_t UpdateCRC(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size);

    //! Maximum size of a buffer managed by this class
    static constexpr uint8_t BufferSizeMax = 255;

    // This class does not allocate any array, it uses a buffer allocated externally
    // Those members are the pointer on the table and the size of this table
    uint8_t* mBufferPtr;
    uint8_t  mBufferSize;

public:

    // Helper functions
    static inline uint8_t  WordLoByte(uint16_t w) { return (uint8_t) (w & 0xff); }
    static inline uint8_t  WordHiByte(uint16_t w) { return (uint8_t) ((w >> 8) & 0xff); }
    static inline uint16_t MakeWord(uint8_t lo, uint8_t hi) { return (uint16_t)( ((uint16_t)lo) | (((uint16_t)hi)<<8) ); }

    //! Prepare packet manager with the following buffer
    Packet(uint8_t* ptr = 0, uint8_t size = 0);

    //! Build a packet with the following data
    void build(uint8_t id, uint8_t instruction, uint8_t params_size, uint8_t* params);

    // Basic getters
    uint8_t  getId()                 const;
    uint16_t getLength()             const;
    uint8_t  getInstruction()        const;
    uint16_t getParameterCount()     const;
    uint8_t  getParameter(uint8_t n) const;
    uint16_t getCrc()                const;

    //! Function to check packet structure
    bool isValid() const;

    //!
    bool isInstructionStatus() const { return (getInstruction() == InstructionStatus); }

    //! Return string representation of the packet
    String toString() const;

    // Function to get the buffer size for those instructions
    static inline uint8_t GetBsPing  ()                                 { return 10; }
    static inline uint8_t GetBsPull  (uint8_t idSize)                   { return (14 + idSize); }
    static inline uint8_t GetBsPush  (uint8_t idSize, uint8_t dataSize) { return (14 + idSize + (dataSize*idSize) ); }
    static inline uint8_t GetBsReset ()                                 { return 10; }
    static inline uint8_t GetBsReboot()                                 { return 10; }

    // Function to fill buffer with data packet
    static void FillBPing  (uint8_t* buffer, uint8_t size);
    static void FillBPull  (  uint8_t* buffer
                            , uint8_t  size
                            , uint8_t* ids      // Table with ids of servos from which you want to read
                            , uint8_t  idSize   // Number of ids in ids table
                            , uint8_t  rAddr    // Address that must be read
                            , uint8_t  rSize    // Size of the register that must be read (1 or 2)
                            );
    static void FillBPush  (  uint8_t* buffer
                            , uint8_t  size
                            , uint8_t* ids
                            , uint8_t  idSize
                            , uint8_t  rAddr
                            , uint8_t  rSize
                            , uint16_t* values
                            );
    static void FillBReset (uint8_t* buffer, uint8_t size);
    static void FillBReboot(uint8_t* buffer, uint8_t size);
};

//! Index for the available register table entry
enum RegIndex
{
    ModelNumber               =   0 ,
    Version                   =   1 ,
    Id                        =   2 ,
    BaudRate                  =   3 ,
    ReturnDelayTime           =   4 ,
    CwAngleLimit              =   5 ,
    CcwAngleLimit             =   6 ,
    ControlMode               =   7 ,
    LimitTemperature          =   8 ,
    DownLimitVoltage          =   9 ,
    UpLimitVoltage            =  10 ,
    MaxTorque                 =  11 ,
    ReturnLevel               =  12 ,
    AlarmShutdown             =  13 ,
    TorqueEnable              =  14 ,
    Led                       =  15 ,
    Dgain                     =  16 ,
    Igain                     =  17 ,
    Pgain                     =  18 ,
    GoalPosition              =  19 ,
    GoalSpeed                 =  20 ,
    GoalTorque                =  21 ,
    PresentPosition           =  22 ,
    PresentSpeed              =  23 ,
    PresentLoad               =  24 ,
    PresentVoltage            =  25 ,
    PresentTemperature        =  26 ,
    RegisteredInstruction     =  27 ,
    Moving                    =  28 ,
    HardwareError             =  29 ,
    Punch                     =  30 ,
    Total                     =  31 ,
};

//! Stored data about each entry of the control table
struct RegInfo
{
    uint8_t addr; // Reg address
    uint8_t size; // Ref size in byte
};

//! Manager to easily control servos
class Controller
{

public:

    //! To easily change serial class without template
    typedef HardwareSerial XlSerial;

    //! Type to store a baudrate value
    typedef uint32_t baudrate_t;

    //! Maximal number of servo xl-320 selectable at once
    static constexpr uint8_t SelectSizeMax = 8;

private:

    //! Size of the Rx buffer, to extends serial buffer
    static constexpr uint16_t RxBufferSizeMax = 128;

    //! Time to wait before stop reading data (in ms)
    static constexpr uint16_t RxBaseTimeout = 1000;

    // Return values for getNextPacket
    static constexpr uint8_t EndOfBuffer = 42;
    static constexpr uint8_t IncompletPk = 21;
    static constexpr uint8_t ValidPacket =  0;

    // Serial controller to comminucate with xl-320
    // and the current baudrate of mXlSerial
    XlSerial*   mXlSerial;
    baudrate_t  mXlSerialBaudRate;

    // mSelectIds : List of servos id selected for next command
    // mSelectSize: Number of usefull element in mSelectIds
    uint8_t mSelectIds[SelectSizeMax];
    uint8_t mSelectSize;

    // mRxPtr       : Current read pointer on the rx buffer, for parsing purpose
    // mRxBuffer    : Buffer for incoming packets
    // mRxBufferSize: Number of bytes in the RxBuffer
    uint8_t* mRxPtr;
    uint8_t  mRxBuffer[RxBufferSizeMax];
    uint16_t mRxBufferSize;

public:

    //! Data about control table
    static const RegInfo RegInfoTable[RegIndex::Total];

    //! Constructor
    Controller();

    //! Initialize the controller with the xl serial stream
    void setup(XlSerial& xlSerial);


    //! Get the xl serial baudrate
    baudrate_t getXlBaudRate() const { return mXlSerialBaudRate; }

    //! Set a new baudrate for the xl serial
    void setXlBaudRate(baudrate_t br);


    //! Return the number of servo selected for actions
    uint8_t getSelectSize() const { return mSelectSize; }

    //! Return a pointer on the table of ids
    const uint8_t* getSelectIds() const { return mSelectIds; }

    //! Select servo ids
    void selectServo(const uint8_t* ids, uint8_t number = 1);


    //! Ping servos and set ids in the table, return the number of answers
    uint8_t ping(uint8_t* ids);

    //! Pull values from servos
    uint8_t pull(RegIndex index, uint16_t* values);

    //! Push values to servos
    void push(RegIndex index, uint16_t* values);

    //! Reset factory for servos
    void reset();

    //! Reboot servos
    void reboot();

private:

    //! Send data from the buffer through serial port
    void sendBuffer(uint8_t* buffer, uint8_t bsize);

    //! Read data from serial during usTimeout micro-secondes
    uint16_t receiveData(uint16_t usTimeout);

    //! Get the next packet in the RxBuffer
    //! Return false if no packet found, else true
    uint8_t getNextPacket(Packet& pack);

    //! just erase data in the rx buffer
    void dropRxData(); 

};

} // xl320

#endif // XL320_HPP
