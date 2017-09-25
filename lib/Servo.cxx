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
int Servo::RegMapSize()
{
    const RegEntry& r = Packet::RegMap[Punch];
    return (r.address + r.size + 1);
}

/* ============================================================================
 *
 * */
RegIndex Servo::RegisterAddr2RegIndex(uint8_t addr)
{
    for(int i=0 ; i<(int)RegIndex::Total ; i++)
    {
        const RegEntry& r = Packet::RegMap[i];
        if(r.address == addr) return (RegIndex)i;
    }
    return RegIndex::Total;
}

/* ============================================================================
 *
 * */
Servo::Servo(uint8_t id, Service* service)
    : mService(service)
    , mId(id)
{
    const int map_size = RegMapSize();
    mData.resize(map_size);
    std::fill(mData.begin(),mData.end(),0);
}

/* ============================================================================
 *
 * */
uint16_t Servo::get(RegIndex index) const
{
    return get(index, mData);
}

/* ============================================================================
 *
 * */
void Servo::set(RegIndex index, uint16_t value)
{
    // log
    log() << "+ Servo::set(" << (int)index << ", " << value << ")";

    // Get entry in the map
    const RegEntry& entry = Packet::RegMap[index];

    // Register the modification
    PushEntry pe = {index, value};
    mRequestedPush.push(pe);
}

/* ============================================================================
 *
 * */
void Servo::pull(const std::list<RegIndex>& indexes)
{
    log() << "+ Servo::pull(queue_size[" << indexes.size() << "])";

    // Check service
    if (!mService)
    {
        log() << "    - Service not set";
        return;
    }

    // Prepare command
    Command command;

    // // Append order for each pull request
    // for(auto index : indexes)
    // {
    //     const RegEntry& entry = Packet::RegMap[index];
    //     log()   << "    - Add order(pull, " << (int)mId << ", "
    //             << (int)entry.address << ", " << (int)entry.size << ")";
    //     command << Command::Order(Command::Type::pull, mId, entry.address, entry.size);
    // }

    // Finally register command
    mService->registerCommand( command );
}

/* ============================================================================
 *
 * */
void Servo::pull(RegIndex index)
{
    std::list<RegIndex> indexes;
    indexes.push_back(index);
    pull(indexes);
}

/* ============================================================================
 *
 * */
void Servo::pullAll()
{
    std::list<RegIndex> indexes;
    indexes.push_back(ModelNumber             );
    indexes.push_back(VersionofFirmware       );
    indexes.push_back(ID                      );
    indexes.push_back(BaudRate                );
    indexes.push_back(ReturnDelayTime         );
    indexes.push_back(CWAngleLimit            );
    indexes.push_back(CCWAngleLimit           );
    indexes.push_back(ControlMode             );
    indexes.push_back(LimitTemperature        );
    indexes.push_back(lowerLimitVoltage       );
    indexes.push_back(UpperLimitVoltage       );
    indexes.push_back(MaxTorque               );
    indexes.push_back(ReturnLevel             );
    indexes.push_back(AlarmShutdown           );
    indexes.push_back(TorqueEnable            );
    indexes.push_back(LED                     );
    indexes.push_back(DGain                   );
    indexes.push_back(IGain                   );
    indexes.push_back(PGain                   );
    indexes.push_back(GoalPosition            );
    indexes.push_back(MovingSpeed             );
    indexes.push_back(TorqueLimit             );
    indexes.push_back(PresentPosition         );
    indexes.push_back(PresentSpeed            );
    indexes.push_back(PresentLoad             );
    indexes.push_back(PresentVoltage          );
    indexes.push_back(PresentTemperature      );
    indexes.push_back(RegisteredInstruction   );
    indexes.push_back(Moving                  );
    indexes.push_back(HardwareErrorStatus     );
    indexes.push_back(Punch                   );
    indexes.push_back(Total                   );
    pull(indexes);       
}

/* ============================================================================
 *
 * */
void Servo::push()
{
    // Log
    log() << "+ Servo::Push()";

    // Check service
    if ( !mService )
    {
        log() << "    - Service not set";
        return;
    }

    // Prepare command
    Command command;

    // // Append order for each push request
    // while(!mRequestedPush.empty())
    // {
    //     PushEntry pentry = mRequestedPush.front();
    //     const RegEntry& entry = Packet::RegMap[pentry.index];

    //     // log
    //     log()   << "    - Add order(push, " << (int)mId << ", "
    //             << (int)entry.address << ", " << (int)entry.size << ", " << (int)pentry.data << ")";

    //     // Append order
    //     command << Command::Order(Command::Type::push, mId, entry.address, entry.size, pentry.data);
    //     mRequestedPush.pop();
    // }

    // Finally register command
    mService->registerCommand( command );
}

/* ============================================================================
 *
 * */
uint16_t Servo::get(RegIndex index, const ByteArray& regTable) const
{
    const RegEntry& entry = Packet::RegMap[index];
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
void Servo::set(RegIndex index, uint16_t value, ByteArray& regTable)
{
    const RegEntry& entry = Packet::RegMap[index];
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
