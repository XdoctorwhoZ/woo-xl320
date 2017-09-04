// woo
#include <woo/arduino-xl320/Controller.h>
#include <woo/arduino-xl320/Service.h>

// ---
using namespace woo::arduino_xl320;

/* ============================================================================
 *
 * */
Controller::Controller(Service* service, const QList<int>& ids)
    : mService(service)
    , mIds(ids)
{ }

/* ============================================================================
 *
 * */
void Controller::setData(Command::Name name, const QString& value)
{

}

/* ============================================================================
 *
 * */
void Controller::requestDataUpdate(Command::Name name)
{
    mService->sendCmd(mIds.getStr(), name, Command::Type::Getter);
}
