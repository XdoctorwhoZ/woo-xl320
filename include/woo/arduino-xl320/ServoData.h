#ifndef WOO_XL320_SERVODATA_H
#define WOO_XL320_SERVODATA_H

// Woo
#include "Command.h"

// ---
namespace woo { namespace arduino_xl320 {

//!
//!
class ServoData
{

    uint16_t mGoalPosition    ;
    uint16_t mGoalSpeed       ;
    uint16_t mGoalTorque      ;
    uint16_t mPresentPosition ;
    uint16_t mPresentSpeed    ;
    uint16_t mPresentLoad     ;
    uint8_t  mPresentVoltage  ;

public:

    ServoData() { }


    void set(Command::Name name, const QByteArray& value);

};


} // arduino_xl320
} // woo

#endif // WOO_XL320_SERVODATA_H
