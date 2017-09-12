#ifndef WOO_XL320_SERVO_H
#define WOO_XL320_SERVO_H
#pragma once

// Qt
#include <QDebug>
#include <QVector>
#include <QByteArray>

// Woo
#include "DllSpec.h"
#include "Command.h"

// ---
namespace woo { namespace xl320 {

// ---
class Service;

//! Class to manage servo data
//!
class Servo
{
    //! Access right
    enum RegisterAccess { NoAccess, ReadOnly, WriteOnly, ReadWrite };

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

    //! Index for each register in the map table
    enum RegisterIndex
    {
        ModelNumber             =  0,
        VersionofFirmware       =  1,
        ID                      =  2,
        BaudRate                =  3,
        ReturnDelayTime         =  4,
        CWAngleLimit            =  5,
        CCWAngleLimit           =  6,
        UNUSED_0                =  7,
        ControlMode             =  8,
        LimitTemperature        =  9,
        lowerLimitVoltage       = 10,
        UpperLimitVoltage       = 11,
        MaxTorque               = 12,
        ReturnLevel             = 13,
        AlarmShutdown           = 14,
        UNUSED_1                = 15,
        TorqueEnable            = 16,
        LED                     = 17,
        UNUSED_2                = 18,
        DGain                   = 19,
        IGain                   = 20,
        PGain                   = 21,
        GoalPosition            = 22,
        MovingSpeed             = 23,
        UNUSED_3                = 24,
        TorqueLimit             = 25,
        PresentPosition         = 26,
        PresentSpeed            = 27,
        PresentLoad             = 28,
        UNUSED_4                = 29,
        PresentVoltage          = 30,
        PresentTemperature      = 31,
        RegisteredInstruction   = 32,
        UNUSED_5                = 33,
        Moving                  = 34,
        HardwareErrorStatus     = 35,
        Punch                   = 36,
    };

    //! Map of registers of the servo
    static const RegisterEntry RegisterMap[];

    //! Return true if the map seams correct
    static bool CheckRegisterMapIntegrity();

    //! Return the size of the all register block
    static int RegisterMapSize();

    // ========================================================================

    // Associated service
    // service*

    // Id
    uint8_t mId;

    //! Image that represents register values inside the servo
    QByteArray mRegisterDistantData;

    //! Image that represents non sync user modifications
    QByteArray mRegisterWorkingData;

public:

    //! Constructor
    Servo();

    //! To extract a value from working registers
    uint16_t get(RegisterIndex index);

    //! To set a value in working registers
    void set(RegisterIndex index, uint16_t value);


// Servo
// Service      // data
// ComDevice
// SerialComDevice : public ComDevice // binary

// packet   // packet

// pull(RegisterIndex index) {  }
// pull(RegisterIndex beg, RegisterIndex end)
// pullAll()

// push()




};


} // xl320
} // woo

#endif // WOO_XL320_SERVO_H
