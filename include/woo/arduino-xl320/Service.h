#ifndef WOO_XL320_SERVICE_H
#define WOO_XL320_SERVICE_H

// #include <unistd.h>

// #include <iostream>

// Qt
#include <QTimer>
#include <QQueue>
#include <QtSerialPort/QSerialPort>

// woo
#include "Command.h"


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
//! Control reception and emittion of data
//!
class TEST_COMMON_DLLSPEC Service : public QObject
{
    Q_OBJECT

    //! If command does not end before this time, command is declared failure
    static constexpr int CommandTimeout = 5000;

    //! Serial configuration
    struct
    {
        //! Serial port device
        QString devName;
        
        //! Serial port controller
        QSerialPort* port;

        // Port configuration
        QString                     portname;
        qint32                      baudrate;
        QSerialPort::DataBits       databits;
        QSerialPort::StopBits       stopbits;
        QSerialPort::Parity         parity;
        QSerialPort::FlowControl    flowctrl;        
    }
    mSerial;

    //! Data to control command execution
    struct
    {
        //! Running command indicator
        //! pass true when command is sent to arduino
        //! pass false when command result has been received
        bool isRunning;

        //! Current selected ids
        QString currentIds;

        //! Commands that must be send to the arduino controller
        QList<Command> queue;

        //! Timer to control command timeout
        QTimer timerOut;        
    }
    mCommandCtrl;

    //! Data to store results
    struct
    {
        // Result for test
        bool test;
        // Result for ping command
        QList<int> ping;

        // map<int, servo result>
    }
    mResult;

public:

    // Constructor
    Service(QObject* qparent = 0);
    ~Service();

    //! To set the serial port name that must be used
    void setDevName(const QString& dev) { mSerial.devName = dev; }

    //! To start serial port and begin using the service
    int start();
    //! To stop the service
    void stop();

    //! Create a controller for the given servos ids
    Controller getController(const QList<int>& ids);

    //! Return true if a command is current executed
    //!
    bool isCommandRunning() const { return mCommandCtrl.isRunning; }

    //! Function to send a command to arduino
    //!
    void registerCommand( const QString& ids = ""
                        , Command::Name name = Command::Name::Test
                        , Command::Type type = Command::Type::None
                        , const QString& value = "");


    // Basic getters
    bool getTestResult() const { return mResult.test; }
    QList<int> getPingResult() const { return mResult.ping; }

private:

    //! Function to reset system after command end
    void endCommand();

    //! Parse data line from arduino
    //!
    void parseData(const QByteArray& data);

    // Parse functions
    void parseDataTest(const QByteArray& data);
    void parseDataGetter(const QByteArray& data);
    void parseDataSetter(const QByteArray& data);
    void parseDataComment(const QByteArray& data);

public slots:

    //! To send test request
    void sendTest();
    void sendPing();

private slots:

    //! Take the next command in the queue and send it
    //! If a command is already running, a timer wil recall this function later
    //!
    void sendNextCommand();

    //! Read data from serial port when they are ready
    //!
    void readData();

    //! To manage command timeout when arduino does not reply
    void manageCommandTimeout();

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
