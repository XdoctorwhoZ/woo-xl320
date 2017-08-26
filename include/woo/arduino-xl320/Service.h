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
namespace woo { namespace xl320 {


//! 
//!
class TEST_COMMON_DLLSPEC Service : public QObject
{
    Q_OBJECT

    //! Serial port device
    QString mDevName;
    
    //! Serial port controller
    QSerialPort* mSerialPort;

    // Port configuration
    QString                     mPortname;
    qint32                      mBaudrate;
    QSerialPort::DataBits       mDatabits;
    QSerialPort::StopBits       mStopbits;
    QSerialPort::Parity         mParity;
    QSerialPort::FlowControl    mFlowctrl;


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
    }
    mCommandCtrl;

    //! Data to store results
    struct
    {
        // Result for test
        int test;
        // Result for ping command
        QList<int> ping;
    }
    mResult;



public:

    Service(QObject* qparent = 0);
    ~Service();

    //! To set the serial port name that must be used
    void setDevName(const QString& dev) { mDevName = dev; }

    //! To start serial port and begin using the service
    int start();
    //! To stop the service
    void stop();

    //! Return true if a command is current executed
    //!
    bool isCommandRunning() const { return mCommandCtrl.isRunning; }

    //! Function to send a command to arduino
    //!
    void registerCommand( const QString& ids = ""
                        , Command::Name name = Command::Name::Test
                        , Command::Type type = Command::Type::None
                        , const QString& value = "");

    // To test communication with arduino
    // bool isTestOver();
    // bool getTestResult();


private:

    //! Parse data line from arduino
    //!
    void parseData(const QByteArray& data);

public slots:

    //! To send test request
    void sendTest();

private slots:

    //! Take the next command in the queue and send it
    //! If a command is already running, a timer wil recall this function later
    //!
    void sendNextCommand();

    //! Read data from serial port when they are ready
    //!
    void readData();

signals:

    //! Emitted when the running command has received an answers or timeout
    //!
    void commandFinish();

    //! Emitted when the result of the test is ready to be read
    //!
    void testResultReady(bool result);

};

} // xl320
} // woo

#endif // WOO_XL320_SERVICE_H
