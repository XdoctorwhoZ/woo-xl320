#ifndef XL320CONTROLLER_HPP
#define XL320CONTROLLER_HPP
#pragma once

// Arduino
#include <Stream.h>
#include <Arduino.h>
#include <HardwareSerial.h>

// Woo
#include "XL320Packet.hpp"

// ---
namespace xl320 {

//! Available baud rates to communicate with xl-320
enum BaudRate { Br9600=0, Br57600=1, Br115200=2, Br1Mbps=3 };

//! Index for the available control table entry
//!
enum ControlIndex
{
    CiModelNumber               =   0 ,
    CiVersion                   =   1 ,
    CiId                        =   2 ,
    CiBaudRate                  =   3 ,
    CiReturnDelayTime           =   4 ,
    CiCwAngleLimit              =   5 ,
    CiCcwAngleLimit             =   6 ,
    CiControlMode               =   7 ,
    CiLimitTemperature          =   8 ,
    CiDownLimitVoltage          =   9 ,
    CiUpLimitVoltage            =  10 ,
    CiMaxTorque                 =  11 ,
    CiReturnLevel               =  12 ,
    CiAlarmShutdown             =  13 ,
    CiTorqueEnable              =  14 ,
    CiLed                       =  15 ,
    CiDgain                     =  16 ,
    CiIgain                     =  17 ,
    CiPgain                     =  18 ,
    CiGoalPosition              =  19 ,
    CiGoalSpeed                 =  20 ,
    CiGoalTorque                =  21 ,
    CiPresentPosition           =  22 ,
    CiPresentSpeed              =  23 ,
    CiPresentLoad               =  24 ,
    CiPresentVoltage            =  25 ,
    CiPresentTemperature        =  26 ,
    CiRegisteredInstruction     =  27 ,
    CiMoving                    =  28 ,
    CiHardwareError             =  29 ,
    CiPunch                     =  30 ,
    CiTotal                     =  31 ,
};

//! Stored data about each entry of the control table
//!
struct ControlReg
{
    byte addr; // Reg address
    byte size; // Ref size in byte
};

//! Controller for xl-320 servo chain
//!
class Controller
{

public:

    // === DEFINITIONS ===

    //! To easily change serial class without template
    typedef HardwareSerial XlSerial;

    //! Maximal number of servo xl-320 selectable at once
    static constexpr byte MaxServoSelectable = 8;

    //! Data about control table
    //!
    static const ControlReg ControlTable[CiTotal];

private:

    // === MEMBERS ===

    //! Serial controller to comminucate with xl-320
    XlSerial* mXlSerial;

    //! Current baudrate of mXlSerial
    BaudRate mXlSerialBaudRate;

    //! Number of selected servo (size of useful id in mSelectedServoIds)
    byte mNumberOfSelectedServo;

    //! Ids of the selected servo
    byte mSelectedServoIds[MaxServoSelectable];

public:
    
    // === Initializers ===

    //! Constructor
    //!
    Controller();

    //! Destructor
    //!
    virtual ~Controller();

    // === xl serial config ===

    //! Initialize the controller with the cmd stream and the xl serial
    //!
    void setup(XlSerial& xlSerial);

    //! Get the xl serial baudrate 
    //!
    BaudRate getXlBaudRate() const;

    //! Get the xl serial baudrate as string 
    //!
    String getXlBaudRateString() const;

    //! Set a new baudrate for the xl serial
    //!
    void setXlBaudRate(BaudRate br);

    // === Select Servos ===

    //! Return the list of selected servo ids in a string form
    //!
    String getSelectedServoString() const;

    //! Select servo ids
    //!
    void selectServo(const byte* ids, byte number = 1);

    // === Packet helpers ===

    //! Read a packet from xl serial, store data in buffer and return number of byte read
    //! msize is the max size of the buffer
    //!
    int readNextPacket(byte* buffer, int msize);

    //! To broadcast a ping packet
    //!
    void sendPingPacket();

    //! To send packet to write in 1 reg of 1 servo
    //!
    void sendWritePacket(byte id, ControlIndex ci, int value) const;

    // === Ping ===

    //! Ping the chain and return the number of servo found
    //! Pass a byte table to get Ids of each servo
    //!
    int ping(byte* ids = 0);

    // === Goal Position ===

    void getGpos(int* positions) const;
    void setGpos(const int* positions, int number) const;

    // === Goal Speed ===

    void getGspeed(int* speeds) const;
    void setGspeed(const int* speeds) const;

};

} // xl320

#endif // XL320CONTROLLER_HPP
