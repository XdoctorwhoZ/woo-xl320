// woo
#include <woo/xl320/Command.h>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
Command::Command(Type type, uint8_t id, uint8_t addr, uint8_t size, const QByteArray& data)
    : mType(type)
    , mId(id)
    , mAddr(addr)
    , mSize(size)
    , mData(data)
{ }

/* ============================================================================
 *
 * */
QByteArray Command::toDataArray()
{
    QByteArray buffer;
    Packet pack(buffer);

    switch(mType)
    {
        case Type::ping:
        {
            pack.build(Packet::Constant::BroadcastId, Packet::Instruction::InsPing, 0);
            break;
        }
        case Type::pull:
        {
            // params
            const int params_size = 4;
            uint8_t params[params_size];

            QByteArray params;
            params += DXL_LOBYTE(addr);
            params += DXL_HIBYTE(addr);
            params += DXL_LOBYTE(size);
            params += DXL_HIBYTE(size);


            pack.build(mId, Packet::Instruction::InsRead, params);
            break;
        }
        case Type::push:
        {

            break;
        }
    }

    return buffer;
}
