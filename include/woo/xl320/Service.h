#ifndef WOO_XL320_SERVICE_H
#define WOO_XL320_SERVICE_H

// Qt
#include <QTimer>
#include <QQueue>

// woo
#include "Servo.h"

// ---
namespace woo { namespace xl320 {

//! Main class of arduino xl320
//! Control reception and transmission of data
//!
class Service : public QObject
{
    Q_OBJECT

    //! Running command indicator
    //! pass true when command is sent
    //! pass false when command result has been received
    bool mIsRunning;

    //! Commands that must be send
    QQueue<Command> mCommandQueue;

    //! Command currently executed
    Command mCurrentCommand;

    //! Buffer to store incomplete packet received
    QByteArray mRxBuffer;

    //! List of ids reached by ping
    QList<uint8_t> mPingResult;

public:

    //! Constructor
    Service(QObject* qparent = 0);

    //! Destructor
    ~Service();

    //! Function to register a command in the queue
    void registerCommand(const Command& cmd);

    //! Return the list of id from ping
    QList<uint8_t> getPingResult() const { return mPingResult; }

private:

    //! Parse packet contained in rx buffer
    void parsePacket();

    //! Use packet information to update service
    void processPacket(const Packet& pack);
    void processPacket_Ping(const Packet& pack);

private slots:

    //! Take the next command in the tx queue and send it
    //! If a command is already running, a timer will call back this function later
    void sendNextCommand();

public slots:

    //! Manage incomming data from device com
    void receiveData(const QByteArray& data);

    //! To send a ping
    void sendPing();

signals:

    //! To send data through com device
    void commandTransmissionRequested(const QByteArray& data);

    // //! Emitted when the running command has received an answers or timeout
    // //!
    // void commandFinish();

    // //! Emitted when the result of the test is ready to be read
    // //!
    // void testResultReady(bool result);

};

} // xl320
} // woo

#endif // WOO_XL320_SERVICE_H
