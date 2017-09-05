#ifndef WOO_XL320_SERVICE_H
#define WOO_XL320_SERVICE_H

// Qt
#include <QTimer>
#include <QQueue>
#include <QtSerialPort/QSerialPort>

// woo
#include "DllSpec.h"
#include "ServiceData.h"
#include "ServiceSerial.h"
#include "ServiceCommand.h"

// ---
namespace woo { namespace arduino_xl320 {

// ---
class Controller;

//! Main class of arduino xl320
//! Control reception and transmission of data
//!
class WOO_ARDUINO_XL320_DLLSPEC Service : public QObject
{
    Q_OBJECT

    //! To store data about the arduino state
    ServiceData mServiceData;

    //! To control serial port
    ServiceSerial mServiceSerial;

    //! To control command transmission and reception + parsing
    ServiceCommand mServiceCommand;

public:

    //! Constructors
    Service(QObject* qparent = 0);
    ~Service();

    //! To set the serial port name that must be used
    void setDevName(const QString& dev) { mServiceSerial.setDevName(dev); }

    //! Must be called before using the service
    int start();
    //! Must be called when the service is not longer used
    void stop();

    //! Function to send a command to arduino
    void sendCmd( const QString& ids = ""
                , Command::Name name = Command::Name::Test
                , Command::Type type = Command::Type::None
                , const QString& value = "")
    { mServiceCommand.registerCommand(ids, name, type, value); }

    //! Create a controller for the given servos ids
    Controller getController(const QList<int>& ids);



    // Basic getters
    // bool getTestResult() const { return mResult.test; }
    // QList<int> getPingResult() const { return mResult.ping; }


public slots:

    //! To send test request
    void sendTest();

    //! To send a ping
    void sendPing();

signals:

    //! Emitted when the running command has received an answers or timeout
    //!
    void commandFinish();

    //! Emitted when the result of the test is ready to be read
    //!
    void testResultReady(bool result);

};

} // arduino_xl320
} // woo

#endif // WOO_XL320_SERVICE_H
