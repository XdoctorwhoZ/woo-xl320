// woo
#include <woo/arduino-xl320/ServiceCommand.h>

// Qt
#include <QDebug>

// ---
using namespace woo::arduino_xl320;

// Uncomment to enable more debug info
#define DEBUG_PLUS 1

/* ============================================================================
 *
 * */
void ServiceCommand::registerCommand( const QString& ids
                             , Command::Name name
                             , Command::Type type
                             , const QString& value)
{
    mTxQueue.push_back(Command(ids, name, type, value));
    QTimer::singleShot(0, this, SLOT(sendNextCommand()) );
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
void ServiceCommand::parseData(const QByteArray& data)
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
    #ifdef DEBUG_PLUS
    qDebug() << "  + parseDataGetter(" << data << ")";
    #endif

    // Getter message
    // +CMD:RESULT_ID0,RESULT_ID1,...,RESULT_ID2\r\n

    // Get ':' index
    const int dotIndex = data.lastIndexOf(':');
    if (dotIndex == -1)
    {
        QString err = QString("%1:%2 Parse Error ':' not found").arg(__FILE__).arg(__LINE__);

        #ifdef DEBUG_PLUS
        qDebug() << "      - " << err;
        #endif 

        emit parseErrorOccured(err);
        return;
    }

    // Get '\r' index
    const int endIndex = data.lastIndexOf('\r');
    if (endIndex == -1)
    {
        QString err = QString("%1:%2 Parse Error '\\r' not found").arg(__FILE__).arg(__LINE__);

        #ifdef DEBUG_PLUS
        qDebug() << "      - " << err;
        #endif 

        emit parseErrorOccured(err);
        return;
    }

    // Extract name and convert to id
    QByteArray cmdNameStr = data.mid(1, dotIndex-1);
    Command::Name cmdName = Command::NameStr2Id(cmdNameStr);
    if (cmdName == Command::Name::Total)
    {
        QString err = QString("%1:%2 Parse Error unknown command '%1'").arg(__FILE__).arg(__LINE__).arg(QString(cmdNameStr));

        #ifdef DEBUG_PLUS
        qDebug() << "      - " << err;
        #endif 

        emit parseErrorOccured(err);
        return;
    }

    // Get result list and export it
    QByteArray cmdResultStr = data.mid(dotIndex+1, (endIndex-(dotIndex+1)));
    QList<QByteArray> results = cmdResultStr.split(',');
    for(int i=0 ; i<mCurIds.getLis().size() ; i++)
    {
        const int id = mCurIds.getLis()[i];

        #ifdef DEBUG_PLUS
        qDebug() << "      - updateReceived(" << id << ", " << cmdNameStr << ", " << results[i] << ")";
        #endif 

        emit updateReceived(id, cmdName, results[i]);
    }

    // End command
    endCommand();
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
