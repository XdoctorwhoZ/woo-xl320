// Woo
#include "XL320.h"

// ---
using namespace xl320;

// Uncomment to debug
#define XL320Controller_DEBUG

/* ============================================================================
 *
 * */
byte SimpleOrder::getRequiredSize()
{
    switch(type)
    {
        case Ping:   return 10;
        case Pull:   return 14;
        case Push:   return 12 + Controller::RegInfoTable[index].size;
        case Reset:  return 10;
        case Reboot: return 10;
    }
}

/* ============================================================================
 *
 * */
void SimpleOrder::fillBuffer(byte* buffer, byte size)
{
    // Build packet
    Packet pack(buffer, size);
    switch(type)
    {
        case Order::Type::Ping:
        {
            pack.build(Constant::BroadcastId, Instruction::Ping, 0);
            break;
        }

        case Order::Type::Pull:
        {
            byte params[4];
            params[0] = DXL_LOBYTE(Controller::RegInfoTable[index].addr);
            params[1] = DXL_HIBYTE(Controller::RegInfoTable[index].addr);
            params[2] = DXL_LOBYTE(Controller::RegInfoTable[index].size);
            params[3] = DXL_HIBYTE(Controller::RegInfoTable[index].size);
            pack.build(id, Instruction::Read, 4, params);
        }

        case Order::Type::Push:
        {
            const int params_size = 2 + Controller::RegInfoTable[index].size;
            byte params[params_size];
            params[0] = DXL_LOBYTE(Controller::RegInfoTable[index].addr);
            params[1] = DXL_HIBYTE(Controller::RegInfoTable[index].addr);
            switch(Controller::RegInfoTable[index].size)
            {
                case 1:
                {
                    params[2] = DXL_LOBYTE(data);
                }
                case 2:
                {
                    params[2] = DXL_LOBYTE(data);
                    params[3] = DXL_HIBYTE(data);
                }
            }
            pack.build(id, Instruction::Write, params_size, params);
        }

        case Order::Type::Reset:
        {
            pack.build(Constant::BroadcastId, Instruction::FactoryReset, 0);
        }

        case Order::Type::Reboot:
        {
            pack.build(Constant::BroadcastId, Instruction::Reboot, 0);
        }
    }
}

/* ============================================================================
 *
 * */
byte MultiOrder::getRequiredSize()
{
    switch(type)
    {
        case Ping:   return 0;
        case Pull:   return 14 + idsSize;
        case Push:   return 14 + idsSize + (idsSize*Controller::RegInfoTable[index].size);
        case Reset:  return 0;
        case Reboot: return 0;
    }
}

/* ============================================================================
 *
 * */
void MultiOrder::fillBuffer(byte* buffer, byte size)
{
    // Build packet
    Packet pack(buffer, size);
    switch(type)
    {

        case Pull:
        {
            const int params_size = 4 + idsSize;
            byte params[params_size];
            params[0] = DXL_LOBYTE(Controller::RegInfoTable[index].addr);
            params[1] = DXL_HIBYTE(Controller::RegInfoTable[index].addr);
            params[2] = DXL_LOBYTE(Controller::RegInfoTable[index].size);
            params[3] = DXL_HIBYTE(Controller::RegInfoTable[index].size);
            for(int i=0 ; i<idsSize ; i++) { params[4+i] = ids[i]; }
            pack.build(Constant::BroadcastId, SyncRead, params_size, params);
        }

        case Push:
        {
            const int params_size = 4 + idsSize;
            byte params[params_size];
            params[0] = DXL_LOBYTE(Controller::RegInfoTable[index].addr);
            params[1] = DXL_HIBYTE(Controller::RegInfoTable[index].addr);
            params[2] = DXL_LOBYTE(Controller::RegInfoTable[index].size);
            params[3] = DXL_HIBYTE(Controller::RegInfoTable[index].size);
            byte next = 4;
            for(int i=0 ; i<idsSize ; i++)
            {
                params[next] = ids[i];
                next++;
                switch(Controller::RegInfoTable[index].size)
                {
                    case 1:
                    {
                        params[next] = DXL_LOBYTE(data[i]);
                        next++;
                    }
                    case 2:
                    {
                        params[next] = DXL_LOBYTE(data[i]);
                        next++;
                        params[next] = DXL_HIBYTE(data[i]);
                        next++;
                    }
                }
            }
            pack.build(Constant::BroadcastId, SyncWrite, params_size, params);
        }

    }
}
