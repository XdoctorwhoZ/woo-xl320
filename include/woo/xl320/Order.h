#ifndef WOO_XL320_ORDER_H
#define WOO_XL320_ORDER_H
#pragma once

// std
#include <queue>

// woo
#include "Packet.h"

// ---
namespace woo { namespace xl320 {

//! Base class for all order
struct Order
{
    virtual ~Order() { }
    virtual Instruction instruction() = 0;
    virtual void buildBuffer(std::vector<uint8_t>& buffer) = 0;
};

//! Order to request ping
struct PingOrder : public Order
{
    Instruction instruction()
    {
        return Instruction::Ping;
    }
    void buildBuffer(std::vector<uint8_t>& buffer)
    {
        Packet::Build(buffer, Packet::BroadcastId, Instruction::Ping);
    }
};


} // xl320
} // woo

#endif // WOO_XL320_ORDER_H
