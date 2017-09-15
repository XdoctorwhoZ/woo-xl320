#ifndef WOO_XL320_SERVO_H
#define WOO_XL320_SERVO_H
#pragma once

// woo
#include "Command.h"

// std
#include <vector>
#include <bitset>
#include <cstring>

// ---
namespace woo { namespace xl320 {

// ---
class Service;

//! Class to manage servo data
class Servo
{

    //!
    friend class Service;

    // helper
    typedef std::vector<uint8_t> ByteArray;

public:

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
        ByteArray   data;
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
    ByteArray mRegisterWorkingData;

    //! When a bit is set to 1 the byte has been modified
    std::bitset<53> mModiflags;
    //! Image that store data that the user want to modify
    ByteArray mRegisterModifiedData;

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
    // void pullAll() { pull(ModelNumber, LED); }

    //! Send local modification to remote servo
    void push();

private:

    //! To extract a value from working registers
    uint16_t get(RegisterIndex index, const ByteArray& regTable) const;

    //! To set a value in working registers
    void set(RegisterIndex index, uint16_t value, ByteArray& regTable);

    //!
    template<class IT>
    void set(RegisterIndex index, IT beg, IT end, ByteArray& regTable)
    {
        const RegisterEntry& entry = RegisterMap[index];
        std::copy(beg, end, regTable.begin()+entry.address);
    }

    //! Update locals and remotes values
    template<class IT>
    void update(RegisterIndex index, IT beg, IT end)
    {
        set(index, beg, end, mRegisterWorkingData);
    }

};


//! ostream support for packet object
inline std::ostream& operator<<(std::ostream& os, const Servo& obj)
{
    int i = 0;
    // os <<
    while (strcmp(Servo::RegisterMap[i].area, "END") != 0)
    {
        const Servo::RegisterEntry& r = Servo::RegisterMap[i];
        os << "(" << r.name << "," << obj.get((Servo::RegisterIndex)r.address) << ")-";
        i++;
    }
    return os;
}


} // xl320
} // woo

#endif // WOO_XL320_SERVO_H