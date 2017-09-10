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

// Std
#include <bitset>

// ---
namespace woo { namespace xl320 {

// ---
class Service;

//! Class to manage servo data
//!
class Servo
{
    //!
    friend class Service;

public :

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

    //! 
    struct ModifEntry
    {
        uint8_t     addr;
        QByteArray  data;
    };

    //! Map of registers of the servo
    static const RegisterEntry RegisterMap[];

    //! Return true if the map seams correct
    static bool CheckRegisterMapIntegrity();

    //! Return the size of the all register block
    static int RegisterMapSize();

public:

    // Associated service
    Service* mService;

    // Id
    uint8_t mId;

    //! Image that represents non sync user modifications
    QByteArray mRegisterWorkingData;

    //! When a bit is set to 1 the byte has been modified
    std::bitset<53> mModiflags;
    //! Image that store data that the user want to modify
    QByteArray mRegisterModifiedData;

    //! Constructor
    Servo(uint8_t id = 1, Service* service = 0);

public:

    // Basic getters
    uint8_t getId() { return mId; }

    //! To extract a value from working registers
    uint16_t get(RegisterIndex index) const;

    //! To set a value in working registers
    void set(RegisterIndex index, uint16_t value);

    //! Get remote value from servo
    void pull(RegisterIndex index) { pull(index, index); }
    void pull(RegisterIndex beg_index, RegisterIndex end_index);
    void pullAll() { pull(ModelNumber, Punch); }

    //! Send local modification to remote servo
    void push();

    //! Get all values dumped in text
    QString toString() const;

private:

    //! To extract a value from working registers
    uint16_t get(RegisterIndex index, const QByteArray& regTable) const;

    //! To set a value in working registers
    void set(RegisterIndex index, uint16_t value, QByteArray& regTable);
    void set(RegisterIndex index, const QByteArray& value, QByteArray& regTable);

    //! Update locals and remotes values
    void update(RegisterIndex index, const QByteArray& value);

};


} // xl320
} // woo

#endif // WOO_XL320_SERVO_H
