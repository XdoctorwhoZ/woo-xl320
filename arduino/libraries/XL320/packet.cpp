// Woo
#include "XL320.h"

// ---
using namespace xl320;

// Uncomment to debug
// #define XL320Packet_DEBUG

/* ============================================================================
 *
 * */
void Packet::build(byte id, Instruction instruction, int params_size, ...)
{
    int packetLength = 3 + params_size;

    mData[0] = Constant::Header0;
    mData[1] = Constant::Header1;
    mData[2] = Constant::Header2;
    mData[3] = Constant::Reserve;
    mData[4] = id;
    mData[5] = DXL_LOBYTE(packetLength);
    mData[6] = DXL_HIBYTE(packetLength);
    mData[7] = (byte)instruction;

    va_list args;
    va_start(args, params_size); 
    for(int i=0 ; i<params_size ; i++)
    {
        byte arg = va_arg(args, int);
        mData[8+i] = arg;
    }
    va_end(args);

    unsigned short crc = update_crc(0, mData, mDataSize-2);
    mData[8+params_size]=DXL_LOBYTE(crc);
    mData[9+params_size]=DXL_HIBYTE(crc);
}

/* ============================================================================
 *
 * */
void Packet::build(byte id, Instruction instruction, int params_size, byte* params)
{
    int packetLength = 3 + params_size;

    mData[0] = Constant::Header0;
    mData[1] = Constant::Header1;
    mData[2] = Constant::Header2;
    mData[3] = Constant::Reserve;
    mData[4] = id;
    mData[5] = DXL_LOBYTE(packetLength);
    mData[6] = DXL_HIBYTE(packetLength);
    mData[7] = (byte)instruction;

    for(int i=0 ; i<params_size ; i++)
    {
        mData[8+i] = params[i];
    }
    
    unsigned short crc = update_crc(0, mData, mDataSize-2);
    mData[8+params_size]=DXL_LOBYTE(crc);
    mData[9+params_size]=DXL_HIBYTE(crc);
}

/* ============================================================================
 *
 * */
int Packet::validate() const
{
    // Check crc
    unsigned short crc0 = getCrc();
    unsigned short crc1 = update_crc(0, mData, mDataSize-2);
    if(crc0 != crc1)
    {
        return 1;
    }

    // OK
    return 0;
}

/* ============================================================================
 *
 * */
String Packet::toString() const
{
    String str;
    str += "id(";
    str += (int) getId();
    str += "),len(";
    str += (int) getLength();
    str += "),ins(";
    str += (int) getInstruction();
    str += "),param(";
    for(int i=0 ; i<this->getParameterCount() ; i++)
    {
        if(i!=0) { str += ','; }
        str += (int) getParameter(i);
    }
    str += "),valid(";
    str += validate();
    str += ")";
    return str;
}
