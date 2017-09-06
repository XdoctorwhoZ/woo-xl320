#ifndef WOO_XL320_SERIALCOMDEVICE_H
#define WOO_XL320_SERIALCOMDEVICE_H
#pragma once

// Qt
#include <QtSerialPort/QSerialPort>

// Woo
#include "ComDevice.h"

// ---
namespace woo { namespace xl320 {

//! Interface for a communication device
class SerialComDevice : public ComDevice
{
    Q_OBJECT

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

    //!
    int start();

    //!
    void stop();

    // basic setters
    void setPortDevice(const QString& dev) { mPortname = dev; }

private slots:

    //! Read data from serial port when they are ready
    void readData();

public slots:

    //! From 
    void sendData(const QByteArray& data);

};

#endif // WOO_XL320_SERIALCOMDEVICE_H
