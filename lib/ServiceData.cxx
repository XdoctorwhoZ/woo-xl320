// woo
#include <woo/arduino-xl320/ServiceData.h>

// Qt
#include <QDebug>

// ---
using namespace woo::arduino_xl320;

/* ============================================================================
 *
 * */
void ServiceData::logParseError(QString error)
{
    mErrors << error;
}

/* ============================================================================
 *
 * */
void ServiceData::updateData(int id, Command::Name cmd, QByteArray value)
{
    mData[id].set(cmd, QString(value).toInt() );
}
