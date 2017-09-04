#ifndef WOO_ARDUINO_XL320_SERVICEDATA_H
#define WOO_ARDUINO_XL320_SERVICEDATA_H
#pragma once

// Qt
#include <QMap>
#include <QTimer>
#include <QQueue>
#include <QObject>

// Woo
#include "ServoData.h"

// ---
namespace woo { namespace arduino_xl320 {

//!
class ServiceData : public QObject
{
    Q_OBJECT

    //! Result for test
    bool mTest;

    //! Result for ping command
    QList<int> mPing;

    //! Other results
    QMap<int, ServoData> mData;

public:
 
    ServiceData() { }
    ~ServiceData() { }

public slots:

    //! Register a parse error
    void logParseError(QString error);

    //! Update data of the servo id
    void updateData(int id, Command::Name cmd, QByteArray vals);

};

} // arduino_xl320
} // woo

#endif // WOO_ARDUINO_XL320_SERVICEDATA_H
