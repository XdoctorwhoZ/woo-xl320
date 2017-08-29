// woo
#include <woo/arduino-xl320/ServiceSerial.h>

// Qt
#include <QDebug>

// ---
using namespace woo::arduino_xl320;

/* ============================================================================
 *
 * */
ServiceSerial::ServiceSerial()
    : mPort(0)
{

}

/* ============================================================================
 *
 * */
ServiceSerial::~ServiceSerial()
{

}

/* ============================================================================
 *
 * */
int ServiceSerial::start()
{
    // Set default configuration
    mPortname = mDevName;
    mBaudrate = 115200;
    mDatabits = QSerialPort::DataBits::Data8;
    mStopbits = QSerialPort::StopBits::OneStop;
    mParity   = QSerialPort::Parity::NoParity;
    mFlowctrl = QSerialPort::FlowControl::NoFlowControl;

    // Create serial port
    if(mPort == 0) {
        mPort = new QSerialPort(this);
        connect(mPort, &QSerialPort::readyRead, this, &ServiceSerial::readData);
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
        return 1;
    }

    // Success
    return 0;
}

/* ============================================================================
 *
 * */
void ServiceSerial::stop()
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
void ServiceSerial::readData()
{
    int size;
    char rBuffer[512];
    while( mPort->canReadLine() )
    {
        // Read the line
        size = mPort->readLine(rBuffer, sizeof(rBuffer));

        // Create an array to parse this line
        QByteArray line(rBuffer, size);

        // Output data
        emit dataLineReceived(line);
    }
}
