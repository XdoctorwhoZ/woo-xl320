#ifndef XL320PACKET_HPP
#define XL320PACKET_HPP
#pragma once

// Arduino
#include <Arduino.h>

// Dynamixel
#include "dxl_pro.h"

// ---
namespace xl320 {

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

//! Return value for the function that validate packet structure
//!
enum PacketState
{
    PsValid       ,
    PsBadHeader   ,
    PsBadCrc      ,
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

//! Packet builder and reader
//!
class Packet
{
    //! Data buffer
    //!
    byte* mData;

    //! Size of the data packet
    //!
    int mDataSize;

public:

    //! Function to compute the size of the buffer
    //!
    static int ComputeBufferSize(int params_size) { return 7 + 3 + params_size; }

    //! Prepare packet manager with the following buffer
    //!
    Packet(byte* buf, int size) : mData(buf), mDataSize(size) { }

    //! Build a packet with the following data
    //!
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
    //!
    PacketState validate() const;

    //! Return string representation of the packet
    //!
    String toString() const;
};

} // xl320

#endif // XL320PACKET_HPP
