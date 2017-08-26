// woo
#include <woo/arduino-xl320/Service.h>
#include <woo/arduino-xl320/Controller.h>

// Qt
#include <QDebug>

// ---
using namespace woo::arduino_xl320;

/* ============================================================================
 *
 * */
Service::Service(QObject* qparent)
    : QObject(qparent)
{
    // Initialize serial port config
    mSerial.port = 0;

    // Initialize command control
    mCommandCtrl.isRunning = false;
    mCommandCtrl.currentIds = "";

    // Init permanent connexion
    connect(&mCommandCtrl.timerOut, &QTimer::timeout, this, &Service::manageCommandTimeout);
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
    mSerial.portname = mSerial.devName;
    mSerial.baudrate = 115200;
    mSerial.databits = QSerialPort::DataBits::Data8;
    mSerial.stopbits = QSerialPort::StopBits::OneStop;
    mSerial.parity   = QSerialPort::Parity::NoParity;
    mSerial.flowctrl = QSerialPort::FlowControl::NoFlowControl;

    // Create serial port
    if(mSerial.port == 0) {
        mSerial.port = new QSerialPort(this);
        connect(mSerial.port, &QSerialPort::readyRead, this, &Service::readData);
    }

    // Set serial configuration
    mSerial.port->setPortName   ( mSerial.portname );
    mSerial.port->setBaudRate   ( mSerial.baudrate );
    mSerial.port->setDataBits   ( mSerial.databits );
    mSerial.port->setStopBits   ( mSerial.stopbits );
    mSerial.port->setParity     ( mSerial.parity   );
    mSerial.port->setFlowControl( mSerial.flowctrl );

    // Open port and check success
    if (!mSerial.port->open(QIODevice::ReadWrite)) {
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
    if(mSerial.port == 0)
    {
        // Close the port
        mSerial.port->close();
        delete mSerial.port;
        mSerial.port = 0;
    }
}

/* ============================================================================
 *
 * */
Controller Service::getController(const QList<int>& ids)
{
    return Controller(this, ids);
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
    registerCommand();
}

/* ============================================================================
 *
 * */
void Service::sendPing()
{
    registerCommand("", Command::Name::Ping, Command::Type::Getter);
}

/* ============================================================================
 *
 * */
void Service::endCommand()
{
    mCommandCtrl.isRunning = false;
    mCommandCtrl.timerOut.stop();
    QTimer::singleShot(0, this, &Service::sendNextCommand);
}

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
            parseDataTest(data);
            qDebug() << "ok - " << data;
            break;
        }
        case '+':
        {
            qDebug() << "++ - " << data;
            parseDataGetter(data);
            break;
        }
        case '=':
        {
            qDebug() << "-- - " << data;
            parseDataSetter(data);
            break;
        }
        case '#':
        {
            qDebug() << "## - " << data;
            parseDataComment(data);
            break;
        }
        default:
        {
            qDebug() << "Unkown? (" << data << ")";
        }
    }
}

/* ============================================================================
 *
 * */
void Service::parseDataTest(const QByteArray& data)
{
    // if( data.size() < 4 ) {

    // }

    // if( )


    // test result ok
    mResult.test = true;

    // End command
    endCommand();
}

/* ============================================================================
 *
 * */
void Service::parseDataGetter(const QByteArray& data)
{
    int dotIndex = data.lastIndexOf(':');
    int endIndex = data.lastIndexOf('\r');

    QByteArray cmdNameStr = data.mid(1, dotIndex-1);
    QByteArray cmdResultStr = data.mid(dotIndex+1, (endIndex-(dotIndex+1)));

    qDebug() << cmdResultStr;
}

/* ============================================================================
 *
 * */
void Service::parseDataSetter(const QByteArray& data)
{

}

/* ============================================================================
 *
 * */
void Service::parseDataComment(const QByteArray& data)
{

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

    // if( cmd.needIdSelection() )
    // {

    // }


    if(cmd.getName() == Command::Name::Test) {
        mResult.test = false;        
    }

    // Command is now running
    mCommandCtrl.isRunning = true;

    // Remove the command that has been sent
    mCommandCtrl.queue.pop_front();

    // Send command
    qDebug() << "send: " << cmd.toData();
    mSerial.port->write(cmd.toData());

    // Start timeout timer
    mCommandCtrl.timerOut.start(CommandTimeout);
}

/* ============================================================================
 *
 * */
void Service::readData()
{
    int size;
    char rBuffer[512];
    while( mSerial.port->canReadLine() )
    {
        // Read the line
        size = mSerial.port->readLine(rBuffer, sizeof(rBuffer));

        // Create an array to parse this line
        QByteArray line(rBuffer, size);

        // Parse data
        parseData(line);
    }
}

/* ============================================================================
 *
 * */
void Service::manageCommandTimeout()
{
    qDebug() << "timeout";
    endCommand();
}
