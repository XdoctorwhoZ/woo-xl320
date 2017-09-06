#ifndef WOO_XL320_COMMAND_H
#define WOO_XL320_COMMAND_H
#pragma once

// Qt
#include <QObject>
#include <QByteArray>

// Woo
#include "Packet.h"

// ---
namespace woo { namespace xl320 {

//!
class Command
{

    enum Type { test, ping, pull, push };
    // addr beg
    // size


    Type mType;

public:

    Command(Type type)
        : mType(type)
    {

    }

    QByteArray toDataStream()
    {

        QByteArray buffer;
        Packet pack(buffer);


        pack.build(Packet::Constant::BroadcastId, Packet::Instruction::InsPing, 0);

        // void Controller::sendPingPacket()
        // {
        //     const int params_size = 0;
        //     const int bsize = Packet::ComputeBufferSize(params_size);

        //     // buffers
        //     byte buffer[bsize];

        //     // Build packet
        //     Packet pack(buffer, bsize);
        //     pack.build(Constant::BroadcastId, InsPing, params_size);

        // #ifdef XL320Controller_DEBUG
        //     Serial.print("#SendPing:size=");
        //     Serial.print(bsize, DEC);
        //     Serial.print(", data=");
        //     Serial.println(pack.toString().c_str());
        // #endif // XL320Controller_DEBUG

        //     // Send packet
        //     mXlSerial->write(buffer,bsize);
        //     mXlSerial->flush();
        // }

    }

};

} // xl320
} // woo

#endif // WOO_XL320_COMMAND_H
