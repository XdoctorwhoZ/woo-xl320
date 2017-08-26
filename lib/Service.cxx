// woo
#include <woo/arduino-xl320/Service.h>
// #include <woo/arduino-xl320/Controller.h>

// Qt
#include <QDebug>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
Service::Service(QObject* qparent)
    : QObject(qparent)
    , mSerialPort(0)
{
    // Initialize command control
    mCommandCtrl.isRunning = false;
    mCommandCtrl.currentIds = "";
}

/* ============================================================================
 *
 * */
Service::~Service()
{
    stop();
}

/* ============================================================================
 *
 * */
int Service::start()
{
    // Set default configuration
    mPortname = mDevName;
    mBaudrate = 115200;
    mDatabits = QSerialPort::DataBits::Data8;
    mStopbits = QSerialPort::StopBits::OneStop;
    mParity   = QSerialPort::Parity::NoParity;
    mFlowctrl = QSerialPort::FlowControl::NoFlowControl;

    // Create serial port
    if(mSerialPort == 0) {
        mSerialPort = new QSerialPort(this);
        connect(mSerialPort, &QSerialPort::readyRead, this, &Service::readData);
    }

    // Set serial configuration
    mSerialPort->setPortName   ( mPortname );
    mSerialPort->setBaudRate   ( mBaudrate );
    mSerialPort->setDataBits   ( mDatabits );
    mSerialPort->setStopBits   ( mStopbits );
    mSerialPort->setParity     ( mParity   );
    mSerialPort->setFlowControl( mFlowctrl );

    // Open port and check success
    if (!mSerialPort->open(QIODevice::ReadWrite)) {
        return 1;
    }

    // Success
    return 0;
}

/* ============================================================================
 *
 * */
void Service::stop()
{
    if(mSerialPort == 0)
    {
        // Close the port
        mSerialPort->close();
        delete mSerialPort;
        mSerialPort = 0;
    }
}

/* ============================================================================
 *
 * */
void Service::registerCommand( const QString& ids
                             , Command::Name name
                             , Command::Type type
                             , const QString& value)
{
    mCommandCtrl.queue.push_back(Command(ids, name, type, value));
    QTimer::singleShot(0, this, &Service::sendNextCommand);
}

/* ============================================================================
 *
 * */
void Service::sendTest()
{
    // Register test command
    registerCommand();
}

/* ============================================================================
 *
 * */
// bool Service::isTestOver()
// {
//     return mIsTestRunning;
// }

// /* ============================================================================
//  *
//  * */
// bool Service::getTestResult()
// {

// }

//  ============================================================================
//  *
//  * 
// void Service::ping()
// {
//     QByteArray command = "XX\r\nXX+PING?\r\n";

//     qDebug() << "PING : " << command;

//     mSerialPort->write(command);
// }

/* ============================================================================
 *
 * */
void Service::parseData(const QByteArray& data)
{
    qDebug() << data;

    // Check if data is not empty
    if (data.isEmpty()) {
        return;
    }
 
    // Get first char
    char firstChar = data[0];
    switch(firstChar)
    {
        case 'O':
        {
            qDebug() << "ok - " << data;
            break;
        }
        case '+':
        {
            qDebug() << "++ - " << data;
            break;
        }
        case '=':
        {
            qDebug() << "-- - " << data;
            break;
        }
        case '#':
        {
            qDebug() << "## - " << data;
            break;
        }
        default:
        {
            qDebug() << "Unkown?" << data;
        }
    }
}

/* ============================================================================
 *
 * */
void Service::sendNextCommand()
{
    qDebug() << "command";

    // No more command to send
    if( mCommandCtrl.queue.isEmpty() ) return;

    // A command is already running
    if( isCommandRunning() ) return;


    const Command& cmd = mCommandCtrl.queue.front();

    if( cmd.needIdSelection() )
    {

    }

    // qDebug() << ;

    mSerialPort->write(cmd.toData());

    // mCommandCtrl.isRunning = true;

}

/* ============================================================================
 *
 * */
void Service::readData()
{
    int size;
    char rBuffer[512];
    while( mSerialPort->canReadLine() )
    {
        // Read the line
        size = mSerialPort->readLine(rBuffer, sizeof(rBuffer));

        // Create an array to parse this line
        QByteArray line(rBuffer, size);

        // Parse data
        parseData(line);
    }
}
