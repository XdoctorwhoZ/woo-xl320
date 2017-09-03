#ifndef WOO_ARDUINO_XL320_SERVICECOMMAND_H
#define WOO_ARDUINO_XL320_SERVICECOMMAND_H
#pragma once

// Qt
#include <QTimer>
#include <QQueue>
#include <QObject>

// Woo
#include "Command.h"
#include "ServoIds.h"

// ---
namespace woo { namespace arduino_xl320 {

//! 
//!
class ServiceCommand : public QObject
{
    Q_OBJECT

    //! Running command indicator
    //! pass true when command is sent to arduino
    //! pass false when command result has been received
    bool mIsRunning;

    //! Timer to control command timeout
    QTimer mTimerOut;

    //! Current selected ids
    ServoIds mCurIds;

    //! Commands that must be send to the arduino controller
    QList<Command> mTxQueue;

public:
 
    ServiceCommand();

    //! Function to register a command in the tx queue
    void registerCommand( const QString& ids = ""
                        , Command::Name name = Command::Name::Test
                        , Command::Type type = Command::Type::None
                        , const QString& value = "");

    //! Function to reset system after command end
    void endCommand();

    // Parse functions
    void parseDataTest(const QByteArray& data);
    void parseDataGetter(const QByteArray& data);
    void parseDataSetter(const QByteArray& data);
    void parseDataComment(const QByteArray& data);

public slots:

    //! Parse 
    void parseInputCmd(const QByteArray& data);

private slots:

    //! Take the next command in the tx queue and send it
    //! If a command is already running, a timer will call again this function later
    void sendNextCommand();

    //! To manage command timeout when arduino does not reply
    void manageCommandTimeout();

signals:

    //! Emitted when the service receive a getter command answer
    //! It will allow the update of internal data 
    void newGetValueReceived(int id, Command::Name name, const QByteArray& value);

};

} // arduino_xl320
} // woo

#endif // WOO_ARDUINO_XL320_SERVICECOMMAND_H