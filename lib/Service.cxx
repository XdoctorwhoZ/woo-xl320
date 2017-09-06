// woo
#include <woo/xl320/Service.h>

// Qt
#include <QDebug>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
Service::Service(QObject* qparent)
    : QObject(qparent)
{

}

/* ============================================================================
 *
 * */
Service::~Service()
{

}

/* ============================================================================
 *
 * */
void Service::parseData(const QByteArray& data)
{

}


/* ============================================================================
 *
 * */
// void Service::sendTest()
// {
//     // registerCommand();
// }

/* ============================================================================
 *
 * */
// void Service::sendPing()
// {
//     // registerCommand("", Command::Name::Ping, Command::Type::Getter);
// }
