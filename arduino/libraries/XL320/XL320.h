#ifndef XL320_HPP
#define XL320_HPP
#pragma once

// Arduino
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAXNUM_TXPACKET     (255)//(65535)
#define MAXNUM_RXPACKET     (255)//(65535)

#define DXL_MAKEWORD(a, b)      ((unsigned short)(((unsigned char)(((unsigned long)(a)) & 0xff)) | ((unsigned short)((unsigned char)(((unsigned long)(b)) & 0xff))) << 8))
#define DXL_MAKEDWORD(a, b)     ((unsigned int)(((unsigned short)(((unsigned long)(a)) & 0xffff)) | ((unsigned int)((unsigned short)(((unsigned long)(b)) & 0xffff))) << 16))
#define DXL_LOWORD(l)           ((unsigned short)(((unsigned long)(l)) & 0xffff))
#define DXL_HIWORD(l)           ((unsigned short)((((unsigned long)(l)) >> 16) & 0xffff))
#define DXL_LOBYTE(w)           ((unsigned char)(((unsigned long)(w)) & 0xff))
#define DXL_HIBYTE(w)           ((unsigned char)((((unsigned long)(w)) >> 8) & 0xff))

#define RX_TIMEOUT_COUNT2       (1600L) //(1000L) //porting
#define NANO_TIME_DELAY         (12000) //ydh added 20111228 -> 20120210 edited ydh
#define RX_TIMEOUT_COUNT1       (RX_TIMEOUT_COUNT2*128L)

unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size);

#ifdef __cplusplus
}
#endif

// ---
namespace xl320 {

//! Available baud rates to communicate with xl-320
enum BaudRate { Br9600=0, Br57600=1, Br115200=2, Br1Mbps=3 };

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
    byte addr; // Reg address
    byte size; // Ref size in byte
};

//! Available instructions
enum Instruction
{
    Ping            = 0x01 , // Instruction that checks whether the Packet has arrived to a device with the same ID as Packet ID
    Read            = 0x02 , // Instruction to read data from the Device
    Write           = 0x03 , // Instruction to write data on the Device
    RegWrite        = 0x04 , // Instruction that registers the Instruction Packet to a standby status; Packet is later executed through the Action command
    Action          = 0x05 , // Instruction that executes the Packet that was registered beforehand using Reg Write
    FactoryReset    = 0x06 , // Instruction that resets the Control Table to its initial factory default settings
    Reboot          = 0x08 , // Instruction to reboot the Device
    Status          = 0x55 , // Return Instruction for the Instruction Packet
    SyncRead        = 0x82 , // (130)For multiple devices, Instruction to read data from the same Address with the same length at once
    SyncWrite       = 0x83 , // (131)For multiple devices, Instruction to write data on the same Address with the same length at once
    BulkRead        = 0x92 , // For multiple devices, Instruction to read data from different Addresses with different lengths at once
    BulkWrite       = 0x93 , // For multiple devices, Instruction to write data on different Addresses with different lengths at once
};

//! Constant value definitions
struct Constant
{
    static constexpr byte Header0 = 0xFF;
    static constexpr byte Header1 = 0xFF;
    static constexpr byte Header2 = 0xFD;
    static constexpr byte Reserve = 0x00;
    static constexpr byte BroadcastId = 0xFE;

    //! Maximal number of servo xl-320 selectable at once
    static constexpr int SelectSizeMax = 8;
};

//! Packet builder and reader
class Packet
{

    //! Data buffer
    byte* mData;

    //! Size of the data packet
    int mDataSize;

public:

    //! Prepare packet manager with the following buffer
    Packet(byte* buf = 0, int size = 0) : mData(buf), mDataSize(size) { }

    //! Build a packet with the following data
    void build(byte id, Instruction instruction, int params_size, ...);
    void build(byte id, Instruction instruction, int params_size, byte* params);

    // Basic getters
    byte getId() const { return mData[4]; }
    int getLength() const { return (int)DXL_MAKEWORD(mData[5], mData[6]); }
    Instruction getInstruction() const { return (Instruction)mData[7]; }
    int getParameterCount() const { return getLength() - 3; }
    byte getParameter(int n) const { return mData[8+n]; }
    unsigned short getCrc() const { return (unsigned short)DXL_MAKEWORD(mData[mDataSize-2], mData[mDataSize-1]); }

    //! Function to check packet structure
    int validate() const;

    //! Return string representation of the packet
    String toString() const;

};

//! Structure to abstract packet creation
struct Order
{
    //! Available order types
    enum Type { Ping, Pull, Push, Reset, Reboot };

    //! Provide the size required to the buffer to contain the order packet
    virtual byte getRequiredSize() = 0;

    //! Fill the buffer with the packet data
    virtual void fillBuffer(byte* buffer, byte size) = 0;
};

//! Simple order to target only one servo and simple request
struct SimpleOrder : public Order
{
    //! Order type
    Type type;
    //! Id of the targeted servo
    byte id;
    //! Index of the targeted register
    RegIndex index;
    //! If type==push, this is the data that must be written
    int data;

    byte getRequiredSize();
    void fillBuffer(byte* buffer, byte size);
};

//! Order to target multiple servos and simple request
struct MultiOrder : public Order
{
    //! Order type
    Type type;
    //! Ids of the targeted servos
    byte idsSize;
    byte ids[Constant::SelectSizeMax];
    //! Index of the targeted register
    RegIndex index;
    //! If type==push, this is the data that must be written
    // byte dataSize; // same as idsSize
    short data[Constant::SelectSizeMax];

    byte getRequiredSize();
    void fillBuffer(byte* buffer, byte size);
};

//! Manager to easily control servos
class Controller
{
    //! To easily change serial class without template
    typedef HardwareSerial XlSerial;

    //! Type to store a baudrate value
    typedef unsigned long baudrate_t;

    //! Size of the Rx buffer, to extends serial buffer
    static constexpr int RxBufferSizeMax = 128;

    //! Time to wait before stop reading data (in ms)
    static constexpr unsigned long RxBaseTimeout = 1000;


    // Serial controller to comminucate with xl-320
    // and the current baudrate of mXlSerial
    XlSerial*   mXlSerial;
    baudrate_t  mXlSerialBaudRate;

    // mSelectIds : List of servos id selected for next command
    // mSelectSize: Number of usefull element in mSelectIds
    byte mSelectIds[Constant::SelectSizeMax];
    byte mSelectSize;

    // mRxPtr       : Current read pointer on the rx buffer
    // mRxBuffer    : Buffer for incoming packets
    // mRxBufferSize: Number of bytes in the RxBuffer
    byte* mRxPtr;
    byte  mRxBuffer[RxBufferSizeMax];
    short mRxBufferSize;

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


    //! Function to execute an order
    void orderExec(Order* order);


    //! Return the number of servo selected for actions
    byte getSelectSize() const { return mSelectSize; }

    //! Return a pointer on the table of ids
    const byte* getSelectIds() const { return mSelectIds; }

    //! Select servo ids
    void selectServo(const byte* ids, byte number = 1);


    //! Read data from serial during usTimeout micro-secondes
    int receiveData(unsigned long usTimeout);

    //! Get the next packet in the RxBuffer
    //! Return false if no packet found, else true
    int getNextPacket(Packet& pack);

    //! just erase data in the rx buffer
    void dropRxData();


    //! Ping servos and set ids in the table, return the number of answers
    byte ping(byte* ids);

    //! Pull values from servos
    byte pull(RegIndex index, int* values);

    //! Push values to servos
    void push(RegIndex index, int* values);

    //! Reset factory for servos
    void reset();

    //! Reboot servos
    void reboot();

};

} // xl320

#endif // XL320_HPP
