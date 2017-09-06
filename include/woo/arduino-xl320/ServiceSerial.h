#ifndef WOO_ARDUINO_XL320_SERVICESERIAL_H
#define WOO_ARDUINO_XL320_SERVICESERIAL_H
#pragma once

// Qt
#include <QObject>
#include <QTimer>
#include <QQueue>
#include <QtSerialPort/QSerialPort>

// ---
namespace woo { namespace arduino_xl320 {

//! Manage serial port configuration and control for the Service class
//! It provides input data for the ServiceCommand
//! ServiceCommand use it to send its ouput commands
//!
class ServiceSerial : public QObject
{
    Q_OBJECT

    //! Serial port device
    QString mDevName;
    
    //! Serial port controller
    QSerialPort* mPort;

    // Port configuration
    QString                     mPortname;
    qint32                      mBaudrate;
    QSerialPort::DataBits       mDatabits;
    QSerialPort::StopBits       mStopbits;
    QSerialPort::Parity         mParity;
    QSerialPort::FlowControl    mFlowctrl;

public:
 
    ServiceSerial();
    ~ServiceSerial();

    void setDevName(const QString& dev) { mDevName = dev; }

    int start();
    void stop();

public slots:

    //! Send data through serial port
    void sendData(const QByteArray& data);

private slots:

    //! Read data from serial port when they are ready
    void readData();

signals:

    //! Emitted when a new line is received from serial port
    void dataLineReceived(const QByteArray& data);

};


} // arduino_xl320
} // woo

#endif // WOO_ARDUINO_XL320_SERVICESERIAL_H
