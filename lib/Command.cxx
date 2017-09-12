// woo
#include <woo/xl320/Command.h>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
Command::Command(Type type, uint8_t id, uint8_t addr, uint8_t size, const std::vector<uint8_t>& data)
    : mType(type)
    , mId(id)
    , mAddr(addr)
    , mSize(size)
    , mData(data)
{ }

/* ============================================================================
 *
 * */
std::vector<uint8_t> Command::toDataArray()
{
    std::vector<uint8_t> buffer;
    Packet pack(buffer);

    switch(mType)
    {
        case Type::ping:
        {
            pack.build(Packet::Constant::BroadcastId, Packet::Instruction::InsPing, 0);
            break;
        }
    //     case Type::pull:
    //     {
    //         std::vector<uint8_t> params;
    //         params += Packet::WordLoByte(mAddr);
    //         params += Packet::WordHiByte(mAddr);
    //         params += Packet::WordLoByte(mSize);
    //         params += Packet::WordHiByte(mSize);
    //         pack.build(mId, Packet::Instruction::InsRead, params);
    //         break;
    //     }
    //     case Type::push:
    //     {

    //         break;
    //     }
    }

    return buffer;
}
