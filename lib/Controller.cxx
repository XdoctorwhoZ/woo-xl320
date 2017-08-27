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
{
    for(int i=0 ; i<mIds.size() ; i++)
    {
        if(i!=0) { mIdsStr += ','; }
        mIdsStr += QString::number(mIds[i]);
    }
}

/* ============================================================================
 *
 * */
void Controller::getRqst(Command::Name name)
{
    mService->registerCommand(mIdsStr, name, Command::Type::Getter);
}
