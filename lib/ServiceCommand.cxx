// woo
#include <woo/arduino-xl320/ServiceCommand.h>

// Qt
#include <QDebug>

// ---
using namespace woo::arduino_xl320;

/* ============================================================================
 *
 * */
ServiceCommand::ServiceCommand()
    : mIsRunning(0)
{
    connect(&mTimerOut, &QTimer::timeout, this, &ServiceCommand::manageCommandTimeout);
}

/* ============================================================================
 *
 * */
void ServiceCommand::registerCommand( const QString& ids
                             , Command::Name name
                             , Command::Type type
                             , const QString& value)
{
    // mCmdCtrl.queue.push_back(Command(ids, name, type, value));
    // QTimer::singleShot(0, this, &ServiceCommand::sendNextCommand);
}

/* ============================================================================
 *
 * */
void ServiceCommand::endCommand()
{
    // mCmdCtrl.isRunning = false;
    // mCmdCtrl.timerOut.stop();
    // QTimer::singleShot(0, this, &ServiceCommand::sendNextCommand);
}

/* ============================================================================
 *
 * */
void ServiceCommand::parseDataTest(const QByteArray& data)
{
    // if( data.size() < 4 ) {

    // }

    // if( )


    // test result ok
    // mResult.test = true;

    // End command
    endCommand();
}

/* ============================================================================
 *
 * */
void ServiceCommand::parseDataGetter(const QByteArray& data)
{
    /*
    // Getter message
    // +CMD:RESULT_ID0,RESULT_ID1,...,RESULT_ID2\r\n

    // Get elements indexes
    const int dotIndex = data.lastIndexOf(':');
    const int endIndex = data.lastIndexOf('\r');

    // Extract strings
    QByteArray cmdNameStr = data.mid(1, dotIndex-1);
    QByteArray cmdResultStr = data.mid(dotIndex+1, (endIndex-(dotIndex+1)));

    // Convert name id
    Command::Name cmdName = Command::NameStr2Id(cmdNameStr);

    // Get result list and store them
    QList<QByteArray> results = cmdResultStr.split(',');
    for(int i=0 ; i<currentIdsList.size() ; i++)
    {
        const int id = currentIdsList[i];
        data[id].set(cmdName, results[i]);

        emit received from arduino(id, cmdname, value)
    }
    
    qDebug() << cmdResultStr;
    
    // End command
    endCommand();
    */
}

/* ============================================================================
 *
 * */
void ServiceCommand::parseDataSetter(const QByteArray& data)
{

}

/* ============================================================================
 *
 * */
void ServiceCommand::parseDataComment(const QByteArray& data)
{

}

/* ============================================================================
 *
 * */
void ServiceCommand::parseInputCmd(const QByteArray& data)
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
        case 'O': parseDataTest(data);      break;
        case '+': parseDataGetter(data);    break;
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
void ServiceCommand::sendNextCommand()
{
    /*
    qDebug() << "command";

    // No more command to send
    if( mCmdCtrl.queue.isEmpty() ) return;

    // A command is already running
    if( isCommandRunning() ) return;


    const Command& cmd = mCmdCtrl.queue.front();

    // if( cmd.needIdSelection() )
    // {

    // }


    if(cmd.getName() == Command::Name::Test) {
        mResult.test = false;        
    }

    // Command is now running
    mCmdCtrl.isRunning = true;

    // Remove the command that has been sent
    mCmdCtrl.queue.pop_front();

    // Send command
    qDebug() << "send: " << cmd.toData();
    mSerial.port->write(cmd.toData());

    // Start timeout timer
    mCmdCtrl.timerOut.start(CommandTimeout);
    */
}

/* ============================================================================
 *
 * */
void ServiceCommand::manageCommandTimeout()
{
    qDebug() << "timeout";
    endCommand();
}
