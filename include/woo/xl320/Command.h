#ifndef WOO_XL320_COMMAND_H
#define WOO_XL320_COMMAND_H
#pragma once

// woo
#include "Packet.h"

// ---
namespace woo { namespace xl320 {

//!
class Command
{

public:

    //! Available type for command
    enum Type
    {
        none,
        ping, // Send a ping to all servos to get available ids
        pull,
        push,
    };

private:

    //! Type of wrok to do
    Type mType;

    //! Id of the servo that must get the info
    uint8_t mId;

    //! Start address to read or write
    uint8_t mAddr;

    //! Number of byte to read or write
    uint8_t mSize;

    //! Data to write
    std::vector<uint8_t> mData;

public:

    //! Constructor
    Command(Type type = none
           , uint8_t id = Packet::Constant::BroadcastId
           , uint8_t addr = 0
           , uint8_t size = 0
           , const std::vector<uint8_t>& data = std::vector<uint8_t>());

    // Basic getters
    Type        getType() const { return mType; }
    uint8_t     getId()   const { return mId;   }
    uint8_t     getAddr() const { return mAddr; }
    uint8_t     getSize() const { return mSize; }
    // QByteArray  getData() const { return mData; }

    //!
    std::vector<uint8_t> toDataArray();
};

} // xl320
} // woo

#endif // WOO_XL320_COMMAND_H
