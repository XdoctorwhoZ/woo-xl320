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



    //! Modification request
    struct PushEntry
    {
        RegIndex index;
        uint16_t data;
    };

    //! Return the size of the all register block
    static int RegMapSize();

    //!
    static RegIndex RegisterAddr2RegIndex(uint8_t addr);

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
    uint16_t get(RegIndex index) const;
    void set(RegIndex index, uint16_t value);

    //! Get remote value from servo
    void pull(const std::list<RegIndex>& indexes);
    void pull(RegIndex index);
    void pullAll();

    //! Send local modification to remote servo
    void push();

private:

    //! To extract a value from working registers
    uint16_t get(RegIndex index, const ByteArray& regTable) const;

    //! To set a value in working registers
    void set(RegIndex index, uint16_t value, ByteArray& regTable);

    //! Update local values
    void update(RegIndex index, uint16_t value) { set(index, value, mData); }

};

//! ostream support for packet object
inline std::ostream& operator<<(std::ostream& os, const Servo& obj)
{
    for(int i=0 ; i<(int)RegIndex::Total ; i++)
    {
        const RegEntry& r = Packet::RegMap[i];
        os << "    - " << r.name << " : " << obj.get((RegIndex)i) << "\r\n";
    }
    return os;
}

} // xl320
} // woo

#endif // WOO_XL320_SERVO_H