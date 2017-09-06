#ifndef WOO_XL320_PACKET_H
#define WOO_XL320_PACKET_H
#pragma once

// Qt
#include <QObject>
#include <QByteArray>

// ---
namespace woo { namespace xl320 {

//! 
class Packet
{

public:
    
    //! Constant value definitions
    struct Constant
    {
        static constexpr uint8_t Header0 = 0xFF;
        static constexpr uint8_t Header1 = 0xFF;
        static constexpr uint8_t Header2 = 0xFD;
        static constexpr uint8_t Reserve = 0x00;

        static constexpr uint8_t BroadcastId = 0xFE;
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
        InsStatus          = 0x55 , // Return Instruction for the Instruction Packet
        InsSyncRead        = 0x82 , // For multiple devices, Instruction to read data from the same Address with the same length at once
        InsSyncWrite       = 0x83 , // For multiple devices, Instruction to write data on the same Address with the same length at once
        InsBulkRead        = 0x92 , // For multiple devices, Instruction to read data from different Addresses with different lengths at once
        InsBulkWrite       = 0x93 , // For multiple devices, Instruction to write data on different Addresses with different lengths at once
    };


private:

    //! Working data buffer
    QByteArray& mData;

public:

    //! Function to compute the size of the buffer
    //!
    static int ComputeBufferSize(int params_size) { return 7 + 3 + params_size; }

    //! Prepare packet manager with the following buffer
    Packet(QByteArray& data) : mData(data) { }

    //! Build a packet with the following data
    void build(uint8_t id, Instruction instruction, int params_size, ...);
    void build(uint8_t id, Instruction instruction, int params_size, uint8_t* params);

    // // Basic getters
    // uint8_t getId() const { return mData[4]; }
    // int getLength() const { return (int)DXL_MAKEWORD(mData[5], mData[6]); }
    // Instruction getInstruction() const { return (Instruction)mData[7]; }
    // int getParameterCount() const { return getLength() - 3; }
    // uint8_t getParameter(int n) const { return mData[8+n]; }
    // unsigned short getCrc() const { return (unsigned short)DXL_MAKEWORD(mData[mDataSize-2], mData[mDataSize-1]); }

    //! Function to check packet structure
    //!
    // PacketState validate() const;

    //! Return string representation of the packet
    //!
    // String toString() const;
};

} // xl320
} // woo

#endif // WOO_XL320_PACKET_H
