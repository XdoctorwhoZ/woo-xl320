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
    Instruction instruction() { return Instruction::Ping; }
    void buildBuffer(std::vector<uint8_t>& buffer) { Packet::Build(buffer, Packet::BroadcastId, Instruction::Ping); }
};

//! Simple order to read a register 'reg' from a specific servo 'id'
struct ReadOrder : public Order
{
    uint8_t  id;
    RegIndex reg;
    ReadOrder(uint8_t id, RegIndex reg) : id(id), reg(reg) { }
    Instruction instruction() { return Instruction::Read; }
    void buildBuffer(std::vector<uint8_t>& buffer);
};

//! Order to write a register
struct WriteOrder : public Order
{
    uint8_t  id;
    RegIndex reg;
    uint16_t value;
    WriteOrder(uint8_t id, RegIndex reg, uint16_t value) : id(id), reg(reg), value(value) { }
    Instruction instruction() { return Instruction::Write; }
    void buildBuffer(std::vector<uint8_t>& buffer);
};

//!
struct SyncReadOrder : public Order
{
    uint8_t  id;
    RegIndex reg;
    SyncReadOrder(uint8_t id, RegIndex reg) : id(id), reg(reg) { }
    Instruction instruction() { return Instruction::SyncRead; }
    void buildBuffer(std::vector<uint8_t>& buffer);
};

//!
struct SyncWriteOrder : public Order
{
    uint8_t  id;
    RegIndex reg;
    uint16_t value;
    SyncWriteOrder(uint8_t id, RegIndex reg, uint16_t value) : id(id), reg(reg), value(value) { }
    Instruction instruction() { return Instruction::SyncWrite; }
    void buildBuffer(std::vector<uint8_t>& buffer);
};

} // xl320
} // woo

#endif // WOO_XL320_ORDER_H
