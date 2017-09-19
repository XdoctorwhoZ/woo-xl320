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
        case Reset:  return 0;
        case Reboot: return 0;
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
        case Ping:
        {
            pack.build(Constant::BroadcastId, InsPing, 0);
            break;
        }
            
        case Pull:
        {
            byte params[4];
            params[0] = DXL_LOBYTE(Controller::RegInfoTable[index].addr);
            params[1] = DXL_HIBYTE(Controller::RegInfoTable[index].addr);
            params[2] = DXL_LOBYTE(Controller::RegInfoTable[index].size);
            params[3] = DXL_HIBYTE(Controller::RegInfoTable[index].size);
            pack.build(id, InsRead, 4, params);
        }

        // case push:   return 12 + RegInfoTable[index].size;
        // case reset:  return 0;
        // case reboot: return 0;
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
        // case Push:   return 12 + Controller::RegInfoTable[index].size;
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
            pack.build(Constant::BroadcastId, InsRead, params_size, params);
        }

    }
}
