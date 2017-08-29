// woo
#include <woo/arduino-xl320/ServoIds.h>

// Qt
#include <QDebug>

// ---
using namespace woo::arduino_xl320;

/* ============================================================================
 *
 * */
void ServoIds::str2lis(const QString& str, QList<sid_t>& lis)
{
    lis.clear();
    QStringList ids = str.split(',');
    for(const auto& id : ids) lis << id.toInt();
}

/* ============================================================================
 *
 * */
void ServoIds::lis2str(const QList<sid_t>& lis, QString& str)
{
    str.clear();
    for(int i=0 ; i<lis.size() ; i++) {
        if(i!=0) { str += ','; }
        const auto& id = lis[i];
        str += QString::number(id);
    }
}
