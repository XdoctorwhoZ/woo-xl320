// woo
#include <woo/xl320/Servo.h>
#include <woo/xl320/Service.h>

// std
#include <stdexcept>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
const Servo::RegisterEntry Servo::RegisterMap[] =
{
    { "EEPROM",  0, 2, "Model Number"           , ReadOnly  , 350        , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "EEPROM",  2, 1, "Version of Firmware"    , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "EEPROM",  3, 1, "ID"                     , ReadWrite , 1          , 0          , 252        } ,
    { "EEPROM",  4, 1, "Baud Rate"              , ReadWrite , 3          , 0          , 3          } ,
    { "EEPROM",  5, 1, "Return Delay Time"      , ReadWrite , 250        , 0          , 254        } ,
    { "EEPROM",  6, 2, "CW Angle Limit"         , ReadWrite , 0          , 0          , 1023       } ,
    { "EEPROM",  8, 2, "CCW Angle Limit"        , ReadWrite , 1023       , 0          , 1023       } ,
    { "EEPROM", 10, 1, "UNUSED"                 , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "EEPROM", 11, 1, "Control Mode"           , ReadWrite , 2          , 1          , 2          } ,
    { "EEPROM", 12, 1, "Limit Temperature"      , ReadWrite , 65         , 0          , 150        } ,
    { "EEPROM", 13, 1, "lower Limit Voltage"    , ReadWrite , 60         , 50         , 250        } ,
    { "EEPROM", 14, 1, "Upper Limit Voltage"    , ReadWrite , 90         , 50         , 250        } ,
    { "EEPROM", 15, 2, "Max Torque"             , ReadWrite , 1023       , 0          , 1023       } ,
    { "EEPROM", 17, 1, "Return Level"           , ReadWrite , 2          , 0          , 2          } ,
    { "EEPROM", 18, 1, "Alarm Shutdown"         , ReadWrite , 3          , 0          , 7          } ,
    { "EEPROM", 19, 5, "UNUSED"                 , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,

    { "RAM"   , 24, 1, "Torque Enable"          , ReadWrite , 0          , 0          , 1          } ,
    { "RAM"   , 25, 1, "LED"                    , ReadWrite , 0          , 0          , 7          } ,
    { "RAM"   , 26, 1, "UNUSED"                 , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 27, 1, "D Gain"                 , ReadWrite , 0          , 0          , 254        } ,
    { "RAM"   , 28, 1, "I Gain"                 , ReadWrite , 0          , 0          , 254        } ,
    { "RAM"   , 29, 1, "P Gain"                 , ReadWrite , 32         , 0          , 254        } ,
    { "RAM"   , 30, 2, "Goal Position"          , ReadWrite , 0xFFFFFFFF , 0          , 1023       } ,
    { "RAM"   , 32, 2, "Moving Speed"           , ReadWrite , 0xFFFFFFFF , 0          , 2047       } ,
    { "RAM"   , 34, 1, "UNUSED"                 , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 35, 2, "Torque Limit"           , ReadWrite , 0xFFFFFFFF , 0          , 1023       } ,
    { "RAM"   , 37, 2, "Present Position"       , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 39, 2, "Present Speed"          , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 41, 2, "Present Load"           , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 43, 2, "UNUSED"                 , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 45, 1, "Present Voltage"        , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 46, 1, "Present Temperature"    , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 47, 1, "Registered Instruction" , ReadOnly  , 0          , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 48, 1, "UNUSED"                 , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 49, 1, "Moving"                 , ReadOnly  , 0          , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 50, 1, "Hardware Error Status"  , ReadOnly  , 0          , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 51, 2, "Punch"                  , ReadWrite , 32         , 0          , 1023       } ,

    { "END"   ,  0, 0, ""                       , NoAccess  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
};

/* ============================================================================
 *
 * */
bool Servo::CheckRegisterMapIntegrity()
{
    // int i = 0;
    // while (strcmp(RegisterMap[i+1].area, "END") != 0)
    // {
    //     const RegisterEntry& r0 = RegisterMap[i];
    //     const RegisterEntry& r1 = RegisterMap[i+1];

    //     // Check if there is no gaps between registers
    //     const uint8_t sum = r0.address + r0.size;
    //     if(sum != r1.address)
    //     {
    //         qCritical("Register map error: missing registers %d+%d!=%d", r0.address, r0.size, r1.address);
    //         return false;
    //     }

    //     i++;
    // }
    // return true;
}

/* ============================================================================
 *
 * */
int Servo::RegisterMapSize()
{
    const RegisterEntry& r = RegisterMap[Punch];
    return (r.address + r.size + 1);
}

/* ============================================================================
 *
 * */
Servo::Servo(uint8_t id, Service* service)
    : mService(service)
    , mId(id)
{
    // if ( ! CheckRegisterMapIntegrity() ) {
    //     throw std::logic_error("woo::xl320::Servo -> map registers is not valid");
    // }
    const int map_size = RegisterMapSize();
    mRegisterWorkingData.resize(map_size);
    std::fill(mRegisterWorkingData.begin(),mRegisterWorkingData.end(),0);

    mModiflags.reset();
    mRegisterModifiedData.resize(map_size);
    std::fill(mRegisterModifiedData.begin(),mRegisterModifiedData.end(),0);
}

/* ============================================================================
 *
 * */
uint16_t Servo::get(RegisterIndex index) const
{
    return get(index, mRegisterWorkingData);
}

/* ============================================================================
 *
 * */
void Servo::set(RegisterIndex index, uint16_t value)
{
    const RegisterEntry& entry = RegisterMap[index];
    set(index, value, mRegisterModifiedData);
    for(uint8_t i=entry.address ; i<entry.address+entry.size ; i++)
    {
        mModiflags.set(i);
    }
}

/* ============================================================================
 *
 * */
void Servo::pull(RegisterIndex beg_index, RegisterIndex end_index)
{
    log() << "+ Servo::Pull " << (int)beg_index << "->" << (int)end_index;

    // Check service
    if (!mService)
    {
        log() << "    - Service not set";
        return;
    }

    // Get entry for each index
    const RegisterEntry& beg_entry = RegisterMap[beg_index];
    const RegisterEntry& end_entry = RegisterMap[end_index];

    // Compute parameters for the command
    const uint8_t addr = beg_entry.address;
    const uint8_t size = (end_entry.address - beg_entry.address) + end_entry.size;

    // Logs for debug
    log() << "    - Id  :" << (int)mId;
    log() << "    - Addr:" << (int)addr;
    log() << "    - Size:" << (int)size;

    // Register command
    mService->registerCommand(
        Command(Command::Type::pull, mId, addr, size)
        );
}

/* ============================================================================
 *
 * */
void Servo::push()
{
    if (!mService) {
        return;
    }

    int i=0;

    auto pushNextBlock = [this, &i]()
    {
        uint8_t addr;
        uint8_t size;

        while( (!mModiflags.test(i)) && (i<53) )
        {
            i++;
        }
        addr = i;

        if(i == 53) { return false; }

        while( (mModiflags.test(i)) && (i<53) )
        {
            i++;
        }
        size = i - addr;

        mService->registerCommand (
            Command ( Command::Type::push
                    , mId
                    , addr
                    , size
                    , mRegisterModifiedData.data() + addr
               )
            );

        if(i == 53) { return true; }
    };


    while( pushNextBlock() ) { };

    // 
    mModiflags.reset();
}

/* ============================================================================
 *
 * */
uint16_t Servo::get(RegisterIndex index, const ByteArray& regTable) const
{
    const RegisterEntry& entry = RegisterMap[index];
    switch(entry.size)
    {
        case 1:
        {
            return regTable[entry.address];
        }
        case 2:
        {
            const uint8_t lo = regTable[entry.address];
            const uint8_t hi = regTable[entry.address + 1];
            return Packet::MakeWord(lo,hi);
        }
    }
}

/* ============================================================================
 *
 * */
void Servo::set(RegisterIndex index, uint16_t value, ByteArray& regTable)
{
    const RegisterEntry& entry = RegisterMap[index];
    switch(entry.size)
    {
        case 1:
        {
            regTable[entry.address] = Packet::WordLoByte(value);
            break;
        }
        case 2:
        {
            regTable[entry.address]     = Packet::WordLoByte(value);
            regTable[entry.address + 1] = Packet::WordHiByte(value);
            break;
        }
    }
}
