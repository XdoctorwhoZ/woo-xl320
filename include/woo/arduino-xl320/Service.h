#ifndef WOO_XL320_SERVICE_H
#define WOO_XL320_SERVICE_H

// #include <unistd.h>

// #include <iostream>

// Qt
#include <QTimer>
#include <QQueue>
#include <QtSerialPort/QSerialPort>

// woo
#include "ServiceData.h"
#include "ServiceSerial.h"
#include "ServiceCommand.h"



#if defined TEST
// #error test
 #define TEST_COMMON_DLLSPEC Q_DECL_EXPORT
#else
// #error pastest
 #define TEST_COMMON_DLLSPEC Q_DECL_IMPORT
#endif

// ---
namespace woo { namespace arduino_xl320 {

// ---
class Controller;


//! Main class of arduino xl320
//! Control reception and transmission of data
//!
class TEST_COMMON_DLLSPEC Service : public QObject
{
    Q_OBJECT

    //! If command does not end before this time, command is declared failure
    static constexpr int CommandTimeout = 5000;

    //! To store data about the arduino state
    ServiceData mServiceData;

    //! To control serial port
    ServiceSerial mServiceSerial;

    //! To control command transmission and reception + parsing
    ServiceCommand mServiceCommand;


public:

    Service(QObject* qparent = 0);
    ~Service();

    //! To set the serial port name that must be used
    void setDevName(const QString& dev) { mServiceSerial.setDevName(dev); }

    int start();
    void stop();

    //! Create a controller for the given servos ids
    Controller getController(const QList<int>& ids);

    //! Return true if a command is current executed
    //!
    // bool isCommandRunning() const { return mCmdCtrl.isRunning; }

    //! Function to send a command to arduino
    //!
    void registerCommand( const QString& ids = ""
                        , Command::Name name = Command::Name::Test
                        , Command::Type type = Command::Type::None
                        , const QString& value = "");


    // Basic getters
    // bool getTestResult() const { return mResult.test; }
    // QList<int> getPingResult() const { return mResult.ping; }


public slots:

    //! To send test request
    void sendTest();
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
