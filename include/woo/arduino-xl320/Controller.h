#ifndef WOO_ARDUINO_XL320_CONTROLLER_H
#define WOO_ARDUINO_XL320_CONTROLLER_H

// Qt
#include <QList>

// ---
namespace woo { namespace arduino_xl320 {

// ---
class Service;

//! Class to control group of servos
//!
class Controller
{
    //! Service used
    Service* mService;

    //! Ids of the controlled servos
    QList<int> mIds;

public:

    //! Constructor
    Controller(Service* service, const QList<int>& ids);

    // set(name , value)
    // get(name)

};




} // arduino_xl320
} // woo

#endif // WOO_ARDUINO_XL320_CONTROLLER_H
