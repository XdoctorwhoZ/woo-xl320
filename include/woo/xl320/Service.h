#ifndef WOO_XL320_SERVICE_H
#define WOO_XL320_SERVICE_H

// Qt
#include <QTimer>
#include <QQueue>
#include <QSharedPointer>

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

    //! Timeout values defintion in ms
    static constexpr int PingTimeout = 5000;
    static constexpr int DefaultTimeout = 5000;

    //! Running command indicator
    //! pass true when command is sent
    //! pass false when command result has been received
    bool mIsRunning;

    //! Timer to control command timeout
    QTimer mTimerOut;

    //! Commands that must be send
    QQueue<Command> mCommandQueue;

    //! Command currently executed
    Command mCurrentCommand;

    //! Buffer to store incomplete packet received
    QByteArray mRxBuffer;

    //! List of ids reached by ping
    QList<uint8_t> mPingResult;

    //! All servos connected to this service
    QList<QSharedPointer<Servo>> mServos;

public:

    //! Constructor
    Service(QObject* qparent = 0);

    //! Destructor
    ~Service();

    //! Function to register a command in the queue
    void registerCommand(const Command& cmd);

    //! Return the list of id from ping
    QList<uint8_t> getPingResult() const { return mPingResult; }

    //! Get servo controller for servo id
    Servo* getServo(uint8_t id);

private:

    //! Parse packet contained in rx buffer
    void parsePacket();

    //! Use packet information to update service
    void processPacket(const Packet& pack);
    void processPacket_Ping(const Packet& pack);
    void processPacket_Pull(const Packet& pack);
    void processPacket_Push(const Packet& pack);

    //! End the command process
    void endCommand();

private slots:

    //! Take the next command in the tx queue and send it
    //! If a command is already running, a timer will call back this function later
    void sendNextCommand();

    //! To manage command timeout
    void manageCommandTimeout();

public slots:

    //! Manage incomming data from device com
    void receiveData(const QByteArray& data);

    //! To send a ping
    void sendPing();

signals:

    //! To send data through com device
    void commandTransmissionRequested(const QByteArray& data);

    //! Emitted during a ping command when an answer is received
    void newPingIdReceived(uint8_t id);

    //! Emitted when the running command has received an answers or timeout
    void commandEnded();

};

} // xl320
} // woo

#endif // WOO_XL320_SERVICE_H
