// woo
#include <woo/arduino-xl320/Controller.h>

// ---
using namespace woo::arduino_xl320;

/* ============================================================================
 *
 * */
Controller::Controller(Service* service, const QList<int>& ids)
    : mService(service)
    , mIds(ids)
{ }
