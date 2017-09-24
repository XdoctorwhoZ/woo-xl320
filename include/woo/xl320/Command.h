#ifndef WOO_XL320_COMMAND_H
#define WOO_XL320_COMMAND_H
#pragma once

// std
#include <queue>
#include <memory>

// woo
#include "Order.h"

// ---
namespace woo { namespace xl320 {

//! A command is a queue of order that must be executed
class Command
{

private:

    //! Queue of orders
    std::queue<Order*> mOrders;

public:

    //! Constructor
    Command() { } ;

    //! Check if there is more order in the command
    bool isOver() const { return mOrders.empty(); }

    //! Append an order in the command
    Command& operator<<(Order* ordr) { mOrders.push(ordr); return *this; }

    //! Extract the next order in the command
    std::shared_ptr<Order> extract() {
        Order* o = mOrders.front(); mOrders.pop(); return std::shared_ptr<Order>(o); }

};

} // xl320
} // woo

#endif // WOO_XL320_COMMAND_H
