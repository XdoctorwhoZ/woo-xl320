#ifndef WOO_XL320_PACKET_H
#define WOO_XL320_PACKET_H
#pragma once

// std
#include <vector>
#include <cstdarg>
#include <cstdint>
#include <iostream>

// ---
namespace woo { namespace xl320 {

//! Packet manager
//!
class Packet
{

public:
    
    //! Constant value definitions
    struct Constant
    {
        static constexpr uint8_t Header0        = 0xFF;
        static constexpr uint8_t Header1        = 0xFF;
        static constexpr uint8_t Header2        = 0xFD;
        static constexpr uint8_t Reserve        = 0x00;
        static constexpr uint8_t BroadcastId    = 0xFE;
    };

    //! Return value for the function that validate packet structure
    enum PacketState
    {
        PsValid       = 0x1 ,
        PsBadHeader   = 0x2 ,
        PsBadCrc      = 0x3 ,
    };

    //! Available instructions
    enum Instruction
    {
        InsPing            = 0x01 , // Instruction that checks whether the Packet has arrived to a device with the same ID as Packet ID
        InsRead            = 0x02 , // Instruction to read data from the Device
        InsWrite           = 0x03 , // Instruction to write data on the Device
        InsRegWrite        = 0x04 , // Instruction that registers the Instruction Packet to a standby status; Packet is later executed through the Action command
        InsAction          = 0x05 , // Instruction that executes the Packet that was registered beforehand using Reg Write
        InsFactoryReset    = 0x06 , // Instruction that resets the Control Table to its initial factory default settings
        InsReboot          = 0x08 , // Instruction to reboot the Device
        InsStatus          = 0x55 , // (85) Return Instruction for the Instruction Packet
        InsSyncRead        = 0x82 , // For multiple devices, Instruction to read data from the same Address with the same length at once
        InsSyncWrite       = 0x83 , // For multiple devices, Instruction to write data on the same Address with the same length at once
        InsBulkRead        = 0x92 , // For multiple devices, Instruction to read data from different Addresses with different lengths at once
        InsBulkWrite       = 0x93 , // For multiple devices, Instruction to write data on different Addresses with different lengths at once
    };

    //! Function to compute the requested size of the buffer for a given number of params
    //! + 7 header size
    //! + 1 instruction
    //! + 2 crc
    //! + params size
    static int ComputeBufferSize(int params_size) { return 7 + 1 + 2 + params_size; }

    // Helper functions
    static inline uint8_t WordLoByte(uint16_t w) { return (uint8_t) (w & 0xff); }
    static inline uint8_t WordHiByte(uint16_t w) { return (uint8_t) ((w >> 8) & 0xff); }
    static inline uint16_t MakeWord(uint8_t lo, uint8_t hi) { return (uint16_t)( ((uint16_t)lo) | (((uint16_t)hi)<<8) ); }

    //! Function to compute crc16 for dynamixel
    static uint16_t UpdateCRC(uint16_t crc_accum, uint8_t* data_blk_ptr, uint16_t data_blk_size);

private:

    //! Working data buffer, read only
    const uint8_t* mData;
    uint32_t mDataSize;

public:

    //! Prepare packet manager with the following buffer
    Packet(const uint8_t* data = 0, uint32_t size = 0) : mData(data), mDataSize(size) { }

    //! Build a packet with the following data
    static void Build(std::vector<uint8_t>& data, uint8_t id, Instruction instruction, int params_size, ...);
    static void Build(std::vector<uint8_t>& data, uint8_t id, Instruction instruction, const std::vector<uint8_t>& params);

    // Basic getters
    uint8_t     getId()             const { return mData[4]; }
    uint16_t    getLength()         const { return MakeWord(mData[5], mData[6]); }
    Instruction getInstruction()    const { return static_cast<Instruction>((uint8_t)mData[7]); }
    // uint16_t    getParameterCount() const { return getLength() - 3; }
    // uint8_t     getParameter(int n) const { return mData[8+n]; }    
    // uint16_t    getCrc()            const { return MakeWord(mData[mData.size()-2], mData[mData.size()-1]); }
    // std::vector<uint8_t>  getReadData()       const { return mData.mid(9, getParameterCount()-1); }

    //! Function to check packet structure
    // PacketState validate() const;

};

//! ostream support for packet object
inline std::ostream& operator<<(std::ostream& os, const Packet& obj)
{
    os  << "packet id(" << (int)obj.getId() 
        << "), len(" << obj.getLength() 
        << "), ins(" << (int)obj.getInstruction() << ")";
    return os;
}

} // xl320
} // woo

#endif // WOO_XL320_PACKET_H
