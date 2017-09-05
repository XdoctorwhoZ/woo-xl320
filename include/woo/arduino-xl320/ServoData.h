#ifndef WOO_ARDUINO_XL320_SERVODATA_H
#define WOO_ARDUINO_XL320_SERVODATA_H
#pragma once

// Woo
#include "Command.h"
#include "ServoIds.h"

// ---
namespace woo { namespace arduino_xl320 {

// Rename in just servo
// data -> registers

// Servo
//  mRegisterData

//  RegisterMap
//  RegisterEntry
    // Area
    // Address (Hexadecimal)
    // Size(byet)
    // Name
    // Description
    // Access
    // Inital Value
    // Min
    // Max


class Servo
{


    //! Access right
    enum RegisterAccess { None, ReadOnly, WriteOnly, ReadWrite };

    //! Structure to represent an entry of the register map
    struct RegisterEntry
    {
        const char*     area;
        uint8_t         address;
        uint8_t         size;
        const char*     name;
        RegisterAccess  access;
        uint32_t        initial_value;
        uint32_t        min;
        uint32_t        max;
    };

    //! Map of registers of the servo
    static RegisterEntry RegisterMap[];


public:


    Servo();
    ~Servo();
    
};



//!
//!
class ServoData
{

    //! Table of data about the servo
    uint32_t mData[Command::Name::Total];

public:

    //! Constructor
    ServoData() { }

    //! To get/set a data value
    uint32_t get(Command::Name name) { return mData[(int)name]; }
    void set(Command::Name name, uint32_t value) { mData[(int)name] = value; }

};


} // arduino_xl320
} // woo

#endif // WOO_ARDUINO_XL320_SERVODATA_H
