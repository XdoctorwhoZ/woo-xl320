// woo
#include <woo/xl320/Command.h>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
Command::Order::Order(Type type, uint8_t id, uint8_t addr, uint8_t size, uint16_t data)
    : type(type)
    , id(id)
    , addr(addr)
    , size(size)
    , data(data)
{ }

/* ============================================================================
 *
 * */
std::vector<uint8_t> Command::Order::toByteArray()
{
    std::vector<uint8_t> buffer;
    switch(type)
    {
        case Type::ping:
        {
            Packet::Build(buffer, Packet::Constant::BroadcastId, Packet::Instruction::InsPing, 0);
            break;
        }
        case Type::pull:
        {
            std::vector<uint8_t> params;
            params.push_back(Packet::WordLoByte(addr));
            params.push_back(Packet::WordHiByte(addr));
            params.push_back(Packet::WordLoByte(size));
            params.push_back(Packet::WordHiByte(size));
            Packet::Build(buffer, id, Packet::Instruction::InsRead, params);
            break;
        }
        case Type::push:
        {
            std::vector<uint8_t> params;
            params.push_back(Packet::WordLoByte(addr));
            params.push_back(Packet::WordHiByte(addr));
            switch(size)
            {
                case  1:
                    params.push_back(Packet::WordLoByte(data));
                    break;
                case  2:
                    params.push_back(Packet::WordLoByte(data));
                    params.push_back(Packet::WordHiByte(data));
                    break;
                default: break;
            }
            Packet::Build(buffer, id, Packet::Instruction::InsWrite, params);
            break;
        }
    }
    return buffer;
}
