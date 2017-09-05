#ifndef WOO_ARDUINO_XL320_CONTROLLER_H
#define WOO_ARDUINO_XL320_CONTROLLER_H
#pragma once

// Qt
#include <QList>

// Woo
#include "DllSpec.h"
#include "Command.h"
#include "ServoData.h"

// ---
namespace woo { namespace arduino_xl320 {

// ---
class Service;

//! Class to control group of servos
//!
class WOO_ARDUINO_XL320_DLLSPEC Controller
{
    //! Service used
    Service* mService;

    //! Ids of the controlled servos
    ServoIds mIds;

public:

    //! Constructor
    Controller(Service* service, const QList<int>& ids);

    //! To send a setter command and modify value inside servo
    void setData(Command::Name name, const QString& value);

    //! To send a getter request and get value of the servo
    void requestDataUpdate(Command::Name name);


    // void getData(Command::Name name);

};

} // arduino_xl320
} // woo

#endif // WOO_ARDUINO_XL320_CONTROLLER_H
