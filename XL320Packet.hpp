#ifndef XL320PACKET_HPP
#define XL320PACKET_HPP
#pragma once

// Woo
#include "XL320Defs.hpp"

// ---
namespace xl320 {

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
