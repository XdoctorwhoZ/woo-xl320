#ifndef XL320DEFS_HPP
#define XL320DEFS_HPP
#pragma once

// Arduino
#include <Arduino.h>

// Dynamixel
#include "dxl_pro.h"

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

//! Available instructions
//!
enum Instruction
{
    InsPing            = 0x01 , // Instruction that checks whether the Packet has arrived to a device with the same ID as Packet ID
    InsRead            = 0x02 , // Instruction to read data from the Device
    InsWrite           = 0x03 , // Instruction to write data on the Device
    InsRegWrite        = 0x04 , // Instruction that registers the Instruction Packet to a standby status; Packet is later executed through the Action command
    InsAction          = 0x05 , // Instruction that executes the Packet that was registered beforehand using Reg Write
    InsFactoryReset    = 0x06 , // Instruction that resets the Control Table to its initial factory default settings
    InsReboot          = 0x08 , // Instruction to reboot the Device
    InsStatus          = 0x55 , // Return Instruction for the Instruction Packet
    InsSyncRead        = 0x82 , // For multiple devices, Instruction to read data from the same Address with the same length at once
    InsSyncWrite       = 0x83 , // For multiple devices, Instruction to write data on the same Address with the same length at once
    InsBulkRead        = 0x92 , // For multiple devices, Instruction to read data from different Addresses with different lengths at once
    InsBulkWrite       = 0x93 , // For multiple devices, Instruction to write data on different Addresses with different lengths at once
};

//! Number that indicate the type of error
//!
enum ErrorNumber
{
    EnResultFail          = 0x01 ,  // When it has failed to process the sent Instruction Packet
    
    EnInstructionError    = 0x02 ,  // When an undefined Instruction has been used
                                    // When Action has been used without Reg Write
    
    EnCRCError            = 0x03 ,  // When the CRC of the sent Packet does not match
    
    EnDataRangeError      = 0x04 ,  // When the Data to be written in the corresponding Address is outside the range of the minimum/maximum value
    
    EnDataLengthError     = 0x05 ,  // When you attempt to write Data that is shorter than the data length of the corresponding Address
                                    //     (ex: when you attempt to only use 2 bytes of a item that has been defined as 4 bytes)
    
    EnDataLimitError      = 0x06 ,  // When the Data to be written in the corresponding Address is outside of the Limit value
    
    EnAccessError         = 0x07 ,  // When you attempt to write a value in an Address that is Read Only or has not been defined
                                    // When you attempt to read a value in an Address that is Write Only or has not been defined
                                    // When you attempt to write a value in the ROM domain while in a state of Torque Enable(ROM Lock)
};

//! To store an hardware error and the id of the servo
//!
struct ErrorData
{
    byte id;
    ErrorNumber data;
};

//! Return value for the function that validate packet structure
//!
enum PacketState
{
    PsValid       = 0x1 ,
    PsBadHeader   = 0x2 ,
    PsBadCrc      = 0x3 ,
};

//! Constant value definitions
//!
struct Constant
{
    static constexpr byte Header0 = 0xFF;
    static constexpr byte Header1 = 0xFF;
    static constexpr byte Header2 = 0xFD;
    static constexpr byte Reserve = 0x00;

    static constexpr byte BroadcastId = 0xFE;
};

} // xl320

#endif // XL320DEFS_HPP
