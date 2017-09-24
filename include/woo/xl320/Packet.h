#ifndef WOO_XL320_PACKET_H
#define WOO_XL320_PACKET_H
#pragma once

// woo
#include "Log.h"
#include "Types.h"

// std
#include <vector>
#include <cstdarg>
#include <iostream>

// ---
namespace woo { namespace xl320 {


//! Packet manager
//!
class Packet
{

    // Constant value definitions
    static constexpr uint8_t Header0        = 0xFF;
    static constexpr uint8_t Header1        = 0xFF;
    static constexpr uint8_t Header2        = 0xFD;
    static constexpr uint8_t Reserve        = 0x00;

public:
    
    // Constant value definitions
    static constexpr uint8_t BroadcastId    = 0xFE;

    //! Return value for the function that validate packet structure
    enum PacketState
    {
        PsValid       = 0x1 ,
        PsBadHeader   = 0x2 ,
        PsBadCrc      = 0x3 ,
    };

    //! Function to compute the requested size of the buffer for a given number of params
    //! + 7 header size
    //! + 1 instruction
    //! + 2 crc
    //! + params size
    static int ComputeBufferSize(int params_size) { return 7 + 1 + 2 + params_size; }

    // Helper functions
    static inline uint8_t  WordLoByte(uint16_t w) { return (uint8_t) (w & 0xff); }
    static inline uint8_t  WordHiByte(uint16_t w) { return (uint8_t) ((w >> 8) & 0xff); }
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
    static void Build(std::vector<uint8_t>& data, uint8_t id, Instruction instruction, const std::vector<uint8_t>& params = std::vector<uint8_t>());

    // Basic getters
    uint8_t     getId()             const { return mData[4]; }
    uint16_t    getLength()         const { return MakeWord(mData[5], mData[6]); }
    Instruction getInstruction()    const { return static_cast<Instruction>((uint8_t)mData[7]); }
    uint16_t    getParameterCount() const { return getLength() - 3; }
    uint8_t     getParameter(int n) const { return mData[8+n]; }    
    uint16_t    getCrc()            const { return MakeWord(mData[mDataSize-2], mData[mDataSize-1]); }

    uint16_t    getReadValue()      const
    {
        switch(getParameterCount())
        {
            case 2:  return MakeWord(mData[9], 0);
            case 3:  return MakeWord(mData[9], mData[10]);
            default: return 42;
        }
    }

    const uint8_t* beginReadData() const { return mData+9; }
    const uint8_t* endReadData()   const { return mData+(9+getParameterCount()-1); }

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
