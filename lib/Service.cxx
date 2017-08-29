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
void Service::registerCommand( const QString& ids
                             , Command::Name name
                             , Command::Type type
                             , const QString& value)
{
    // mCmdCtrl.queue.push_back(Command(ids, name, type, value));
    // QTimer::singleShot(0, this, &Service::sendNextCommand);
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
