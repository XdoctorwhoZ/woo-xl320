#ifndef WOO_XL320_TYPES_H
#define WOO_XL320_TYPES_H
#pragma once

// std
#include <cstdint>

// ---
namespace woo { namespace xl320 {

//! Available instructions
enum Instruction : uint8_t
{
    Ping            = 0x01 , // Instruction that checks whether the Packet has arrived to a device with the same ID as Packet ID
    Read            = 0x02 , // Instruction to read data from the Device
    Write           = 0x03 , // Instruction to write data on the Device
    RegWrite        = 0x04 , // Instruction that registers the Instruction Packet to a standby status; Packet is later executed through the Action command
    Action          = 0x05 , // Instruction that executes the Packet that was registered beforehand using Reg Write
    FactoryReset    = 0x06 , // Instruction that resets the Control Table to its initial factory default settings
    Reboot          = 0x08 , // Instruction to reboot the Device
    Status          = 0x55 , // (85) Return Instruction for the Instruction Packet
    SyncRead        = 0x82 , // For multiple devices, Instruction to read data from the same Address with the same length at once
    SyncWrite       = 0x83 , // For multiple devices, Instruction to write data on the same Address with the same length at once
    BulkRead        = 0x92 , // For multiple devices, Instruction to read data from different Addresses with different lengths at once
    BulkWrite       = 0x93 , // For multiple devices, Instruction to write data on different Addresses with different lengths at once
};

//! Access right
enum RegAccess
{
    NoAccess,
    ReadOnly,
    WriteOnly,
    ReadWrite
};

//! Structure to represent an entry of the register map
struct RegEntry
{
    const char*     area;
    uint8_t         address;
    uint8_t         size;
    const char*     name;
    RegAccess       access;
    uint32_t        initial_value;
    uint32_t        min;
    uint32_t        max;
};

//! Index for each register in the map table
enum RegIndex
{
    ModelNumber             =  0,
    VersionofFirmware       =  1,
    ID                      =  2,
    BaudRate                =  3,
    ReturnDelayTime         =  4,
    CWAngleLimit            =  5,
    CCWAngleLimit           =  6,
    UNUSED_0                =  7,
    ControlMode             =  8,
    LimitTemperature        =  9,
    lowerLimitVoltage       = 10,
    UpperLimitVoltage       = 11,
    MaxTorque               = 12,
    ReturnLevel             = 13,
    AlarmShutdown           = 14,
    UNUSED_1                = 15,
    TorqueEnable            = 16,
    LED                     = 17,
    UNUSED_2                = 18,
    DGain                   = 19,
    IGain                   = 20,
    PGain                   = 21,
    GoalPosition            = 22,
    MovingSpeed             = 23,
    UNUSED_3                = 24,
    TorqueLimit             = 25,
    PresentPosition         = 26,
    PresentSpeed            = 27,
    PresentLoad             = 28,
    UNUSED_4                = 29,
    PresentVoltage          = 30,
    PresentTemperature      = 31,
    RegisteredInstruction   = 32,
    UNUSED_5                = 33,
    Moving                  = 34,
    HardwareErrorStatus     = 35,
    Punch                   = 36,
    Total                   = 37,
};

} // xl320
} // woo

#endif // WOO_XL320_TYPES_H
