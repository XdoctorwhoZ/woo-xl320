#ifndef WOO_XL320_SERVO_H
#define WOO_XL320_SERVO_H
#pragma once

// woo
#include "Command.h"

// std
#include <list>
#include <vector>
#include <bitset>
#include <cstring>
#include <numeric>

// ---
namespace woo { namespace xl320 {

// ---
class Service;

//! Class to manage servo data and facilitate the use of service commands
//!
//! It seems that max len of reply packet is 50 bytes...
//! if I request too much, I don't get all the data
//! 
//! It also seems that if I request more than 1 reg at a time
//! I get correct value only for the first, Therefore I go with 1 request at time
//!
class Servo
{

    //! Only service can use some internal method of Servo
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
        Total                   = 37,
    };

    //! Modification request
    struct PushEntry
    {
        RegisterIndex index;
        uint16_t      data;
    };

    //! Map of registers of the servo
    static const RegisterEntry RegisterMap[];

    //! Return true if the map seams correct
    static bool CheckRegisterMapIntegrity();

    //! Return the size of the all register block
    static int RegisterMapSize();

    //!
    static RegisterIndex RegisterAddr2RegisterIndex(uint8_t addr);

public:

    //! Associated service
    Service* mService;

    //! Id
    uint8_t mId;

    //! Image that represents servo registers
    ByteArray mData;

    //! Modification requested by user but not put already
    std::queue<PushEntry> mRequestedPush;

    //! Constructor
    Servo(uint8_t id = 1, Service* service = 0);

    // Basic getters
    uint8_t getId() { return mId; }

    // Getter and setter for local values
    uint16_t get(RegisterIndex index) const;
    void set(RegisterIndex index, uint16_t value);

    //! Get remote value from servo
    void pull(const std::list<RegisterIndex>& indexes);
    void pull(RegisterIndex index);
    void pullAll();

    //! Send local modification to remote servo
    void push();

private:

    //! To extract a value from working registers
    uint16_t get(RegisterIndex index, const ByteArray& regTable) const;

    //! To set a value in working registers
    void set(RegisterIndex index, uint16_t value, ByteArray& regTable);

    //! Update local values
    void update(RegisterIndex index, uint16_t value) { set(index, value, mData); }

};

//! ostream support for packet object
inline std::ostream& operator<<(std::ostream& os, const Servo& obj)
{
    for(int i=0 ; i<(int)Servo::RegisterIndex::Total ; i++)
    {
        const Servo::RegisterEntry& r = Servo::RegisterMap[i];
        os << "    - " << r.name << " : " << obj.get((Servo::RegisterIndex)i) << "\r\n";
    }
    return os;
}

} // xl320
} // woo

#endif // WOO_XL320_SERVO_H