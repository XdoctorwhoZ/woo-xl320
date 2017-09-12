#ifndef WOO_XL320_SERIALCOMDEVICE_H
#define WOO_XL320_SERIALCOMDEVICE_H
#pragma once

// Qt


// ---
namespace woo { namespace xl320 {

//! Interface for a communication device
class SerialComDevice
{



public:

    //! Constructor
    SerialComDevice(const QString& dev = "/dev/ttyACM0", qint32 baud = 115200);

    //! Destructor
    ~SerialComDevice();

    // Basic setters
    void setDevice(const QString& value) { mPortname = value; }
    void setBaudRate(qint32 value) { mBaudrate = value; }

    //! Open serial port 
    int start();

    //! Cleanup object for next opening
    void stop();

private slots:

    //! Read data from serial port when they are ready
    void readData();

public slots:

    //! From ComDevice
    void sendData(const QByteArray& data) { mPort->write(data); }

signals:

    //! Emitted when data is received in com device
    void dataReceived(const QByteArray& data);

};

} // xl320
} // woo

#endif // WOO_XL320_SERIALCOMDEVICE_H
