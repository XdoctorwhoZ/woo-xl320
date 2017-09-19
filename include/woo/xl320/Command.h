#ifndef WOO_XL320_COMMAND_H
#define WOO_XL320_COMMAND_H
#pragma once

// std
#include <queue>

// woo
#include "Packet.h"

// ---
namespace woo { namespace xl320 {

//! A command is a queue of order that must be executed
class Command
{

public:

    //! Available type for command
    enum Type
    {
        none,
        ping,   // Send a ping to all servos to get available ids
        pull,   // Read data
        push,   // Write data
        freset, // Factory reset 
        reboot, // Reboot
    };

    //! Simple element of a command
    struct Order
    {
        //! Type of wrok to do
        Type type;
        //! Ids of the servos targeted by the order
        std::vector<uint8_t> ids;
        //! Start address to read or write
        uint8_t addr;
        //! Number of byte to read or write
        uint8_t size;
        //! Data to write for each servo id
        std::vector<uint16_t> data;

        //! Constructor
        Order(Type type = none
               , uint8_t  id = Packet::Constant::BroadcastId
               , uint8_t  addr = 0
               , uint8_t  size = 0
               , uint16_t data = 0
               );
        Order(Type type
               , const std::vector<uint8_t>& ids
               , uint8_t  addr = 0
               , uint8_t  size = 0
               , const std::vector<uint16_t>& data = std::vector<uint16_t>()
               );

        //! Build the instruction packet ready to be sent through serial port
        std::vector<uint8_t> toByteArray();
    };

private:

    //! Queue of orders
    std::queue<Order> mOrders;

public:

    //! Constructor
    Command() { } ;

    //! Check if there is more order in the command
    bool isOver() const { return mOrders.empty(); }

    //! Append an order in the command
    Command& operator<<(const Order& ordr) { mOrders.push(ordr); return *this; }

    //! Extract the next order in the command
    Order extract() { Order o = mOrders.front(); mOrders.pop(); return o; }

};

} // xl320
} // woo

#endif // WOO_XL320_COMMAND_H
