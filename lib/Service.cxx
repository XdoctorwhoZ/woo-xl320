// woo
#include <woo/arduino-xl320/Service.h>
#include <woo/arduino-xl320/Controller.h>

// Qt
#include <QDebug>

// ---
using namespace woo::arduino_xl320;

/* ============================================================================
 *
 * */
Service::Service(QObject* qparent)
    : QObject(qparent)
{
    // // Initialize command control
    // mCmdCtrl.isRunning = false;
    // mCmdCtrl.currentIds = "";

    // Init permanent connexion
    // connect(&mCmdCtrl.timerOut, &QTimer::timeout, this, &Service::manageCommandTimeout);


    // Connect services each other
    connect(&mServiceCommand, &ServiceCommand::updateReceived, &mServiceData, &ServiceData::updateData);
    connect(&mServiceCommand, &ServiceCommand::parseErrorOccured, &mServiceData, &ServiceData::logParseError);
}

/* ============================================================================
 *
 * */
Service::~Service()
{
    stop();
}

/* ============================================================================
 *
 * */
int Service::start()
{
    return mServiceSerial.start();
}

/* ============================================================================
 *
 * */
void Service::stop()
{
    
}

/* ============================================================================
 *
 * */
Controller Service::getController(const QList<int>& ids)
{
    return Controller(this, ids);
}


/* ============================================================================
 *
 * */
void Service::sendTest()
{
    // registerCommand();
}

/* ============================================================================
 *
 * */
void Service::sendPing()
{
    // registerCommand("", Command::Name::Ping, Command::Type::Getter);
}
