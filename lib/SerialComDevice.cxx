// woo
#include <woo/xl320/SerialComDevice.h>

// Qt
#include <QDebug>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
SerialComDevice::SerialComDevice(const QString& dev, qint32 baud)
    : mPort(0)
{
    // Set default configuration
    mPortname = dev;
    mBaudrate = baud;
    mDatabits = QSerialPort::DataBits::Data8;
    mStopbits = QSerialPort::StopBits::OneStop;
    mParity   = QSerialPort::Parity::NoParity;
    mFlowctrl = QSerialPort::FlowControl::NoFlowControl;
}

/* ============================================================================
 *
 * */
SerialComDevice::~SerialComDevice()
{
    stop();
}

/* ============================================================================
 *
 * */
int SerialComDevice::start()
{
    // Create serial port
    if (mPort == 0) {
        mPort = new QSerialPort(this);
        connect(mPort, &QSerialPort::readyRead, this, &SerialComDevice::readData);
    }

    // Set serial configuration
    mPort->setPortName   ( mPortname );
    mPort->setBaudRate   ( mBaudrate );
    mPort->setDataBits   ( mDatabits );
    mPort->setStopBits   ( mStopbits );
    mPort->setParity     ( mParity   );
    mPort->setFlowControl( mFlowctrl );

    // Open port and check success
    if (!mPort->open(QIODevice::ReadWrite)) {
        qWarning() << "Unable to open port";
        return 1;
    }

    // Success
    return 0;
}

/* ============================================================================
 *
 * */
void SerialComDevice::stop()
{
    // Close serial port
    if(mPort == 0)
    {
        mPort->close();
        delete mPort;
        mPort = 0;
    }
}

/* ============================================================================
 *
 * */
void SerialComDevice::readData()
{
    emit dataReceived(mPort->readAll());
}
