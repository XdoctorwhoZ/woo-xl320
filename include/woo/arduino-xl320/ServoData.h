#ifndef WOO_XL320_SERVODATA_H
#define WOO_XL320_SERVODATA_H

// Woo
#include "Command.h"
#include "ServoIds.h"


// ---
namespace woo { namespace arduino_xl320 {

struct ServoData
{

    uint16_t goalPosition    ;
    uint16_t goalSpeed       ;
    uint16_t goalTorque      ;
    uint16_t presentPosition ;
    uint16_t presentSpeed    ;
    uint16_t presentLoad     ;
    uint8_t  presentVoltage  ;


    void set(Command::Name name, const QByteArray& value);

};


} // arduino_xl320
} // woo

#endif // WOO_XL320_SERVODATA_H
