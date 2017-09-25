// woo
#include <woo/xl320/Order.h>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
void ReadOrder::buildBuffer(std::vector<uint8_t>& buffer)
{
    std::vector<uint8_t> params;
    params.push_back(Packet::WordLoByte(Packet::RegMap[reg].address));
    params.push_back(Packet::WordHiByte(Packet::RegMap[reg].address));
    params.push_back(Packet::WordLoByte(Packet::RegMap[reg].size));
    params.push_back(Packet::WordHiByte(Packet::RegMap[reg].size));
    Packet::Build(buffer, id, Instruction::Read, params);  
}

/* ============================================================================
 *
 * */
void WriteOrder::buildBuffer(std::vector<uint8_t>& buffer)
{
    std::vector<uint8_t> params;
    params.push_back(Packet::WordLoByte(Packet::RegMap[reg].address));
    params.push_back(Packet::WordHiByte(Packet::RegMap[reg].address));
    switch(Packet::RegMap[reg].size)
    {
        case  1:
            params.push_back(Packet::WordLoByte(value));
            break;
        case  2:
            params.push_back(Packet::WordLoByte(value));
            params.push_back(Packet::WordHiByte(value));
            break;
        default: break;
    }
    Packet::Build(buffer, id, Instruction::Write, params);  
}

/* ============================================================================
 *
 * */
void SyncReadOrder::buildBuffer(std::vector<uint8_t>& buffer)
{
    //                 std::vector<uint8_t> params;
//                 params.push_back(Packet::WordLoByte(addr));
//                 params.push_back(Packet::WordHiByte(addr));
//                 params.push_back(Packet::WordLoByte(size));
//                 params.push_back(Packet::WordHiByte(size));
//                 params.insert(params.end(), ids.begin(), ids.end());
//                 Packet::Build(buffer, Packet::Constant::BroadcastId, Packet::Instruction::InsSyncRead, params);
}

/* ============================================================================
 *
 * */
void SyncWriteOrder::buildBuffer(std::vector<uint8_t>& buffer)
{
//                 std::vector<uint8_t> params;
//                 params.push_back(Packet::WordLoByte(addr));
//                 params.push_back(Packet::WordHiByte(addr));
//                 params.push_back(Packet::WordLoByte(size));
//                 params.push_back(Packet::WordHiByte(size));
//                 // for(uint8_t i : ids)
//                 // {
//                 //     params.push_back(i);
//                 //     switch(size)
//                 //     {
//                 //         case  1:
//                 //             params.push_back(Packet::WordLoByte(data.front()));
//                 //             break;
//                 //         case  2:
//                 //             params.push_back(Packet::WordLoByte(data.front()));
//                 //             params.push_back(Packet::WordHiByte(data.front()));
//                 //             break;
//                 //         default: break;
//                 //     }
//                 // }
//                 // params.insert(params.end(), ids.begin(), ids.end());

//                 Packet::Build(buffer, Packet::Constant::BroadcastId, Packet::Instruction::InsSyncWrite, params);
}
