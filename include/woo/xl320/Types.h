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

} // xl320
} // woo

#endif // WOO_XL320_TYPES_H
