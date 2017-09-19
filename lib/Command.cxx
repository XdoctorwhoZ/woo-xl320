// woo
#include <woo/xl320/Command.h>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
Command::Order::Order(Type type, uint8_t id, uint8_t addr, uint8_t size, uint16_t data)
    : type(type)
    , addr(addr)
    , size(size)
{
    this->ids.push_back(id);
    this->data.push_back(data);
}

/* ============================================================================
 *
 * */
Command::Order::Order(Type type, const std::vector<uint8_t>& ids, uint8_t  addr, uint8_t  size
                    , const std::vector<uint16_t>& data)
    : type(type)
    , ids(ids)
    , addr(addr)
    , size(size)
    , data(data)
{

}

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
            if(ids.size() == 1)
            {
                std::vector<uint8_t> params;
                params.push_back(Packet::WordLoByte(addr));
                params.push_back(Packet::WordHiByte(addr));
                params.push_back(Packet::WordLoByte(size));
                params.push_back(Packet::WordHiByte(size));
                Packet::Build(buffer, ids.front(), Packet::Instruction::InsRead, params);              
            }
            else
            {
                std::vector<uint8_t> params;
                params.push_back(Packet::WordLoByte(addr));
                params.push_back(Packet::WordHiByte(addr));
                params.push_back(Packet::WordLoByte(size));
                params.push_back(Packet::WordHiByte(size));
                params.insert(params.end(), ids.begin(), ids.end());
                Packet::Build(buffer, Packet::Constant::BroadcastId, Packet::Instruction::InsSyncRead, params);
            }
            break;
        }
        case Type::push:
        {
            if(ids.size() == 1)
            {
                std::vector<uint8_t> params;
                params.push_back(Packet::WordLoByte(addr));
                params.push_back(Packet::WordHiByte(addr));
                switch(size)
                {
                    case  1:
                        params.push_back(Packet::WordLoByte(data.front()));
                        break;
                    case  2:
                        params.push_back(Packet::WordLoByte(data.front()));
                        params.push_back(Packet::WordHiByte(data.front()));
                        break;
                    default: break;
                }
                Packet::Build(buffer, ids.front(), Packet::Instruction::InsWrite, params);
            }
            else
            {
                std::vector<uint8_t> params;
                params.push_back(Packet::WordLoByte(addr));
                params.push_back(Packet::WordHiByte(addr));
                params.push_back(Packet::WordLoByte(size));
                params.push_back(Packet::WordHiByte(size));
                // for(uint8_t i : ids)
                // {
                //     params.push_back(i);
                //     switch(size)
                //     {
                //         case  1:
                //             params.push_back(Packet::WordLoByte(data.front()));
                //             break;
                //         case  2:
                //             params.push_back(Packet::WordLoByte(data.front()));
                //             params.push_back(Packet::WordHiByte(data.front()));
                //             break;
                //         default: break;
                //     }
                // }
                // params.insert(params.end(), ids.begin(), ids.end());

                Packet::Build(buffer, Packet::Constant::BroadcastId, Packet::Instruction::InsSyncWrite, params);
            }
            break;
        }
    }
    return buffer;
}
