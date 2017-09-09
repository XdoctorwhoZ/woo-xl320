// woo
#include <woo/xl320/Service.h>

// Qt
#include <QDebug>

// ---
using namespace woo::xl320;

// Uncomment to enable more debug info
#define DEBUG_PLUS 1

/* ============================================================================
 *
 * */
Service::Service(QObject* qparent)
    : QObject(qparent)
    , mIsRunning(false)
{
    connect(&mTimerOut, &QTimer::timeout, this, &Service::manageCommandTimeout);
}

/* ============================================================================
 *
 * */
Service::~Service()
{

}

/* ============================================================================
 *
 * */
void Service::registerCommand(const Command& cmd)
{
    mCommandQueue.enqueue(cmd);
    QTimer::singleShot(0, this, SLOT(sendNextCommand()));
}

/* ============================================================================
 *
 * */
Servo* Service::getServo(uint8_t id)
{
    for(auto& s : mServos)
    {
        if(s->getId() == id)
        {
            return s.data();
        }
    }
    QSharedPointer<Servo> servoPtr(new Servo(id, this));
    mServos << servoPtr;
    return servoPtr.data();
}

/* ============================================================================
 *
 * */
void Service::parsePacket()
{
    #ifdef DEBUG_PLUS
    qDebug() << "  + Service::parsePacket(" << mRxBuffer << ")(" << mRxBuffer.size() << ")";
    #endif

    // Check if the buffer is not empty
    if (mRxBuffer.isEmpty())
    {
        #ifdef DEBUG_PLUS
        qDebug() << "      - Rx buffer is empty";
        #endif
        return;
    }

    // Initialize packet size
    uint32_t size = 1;
    uint32_t index = 0;

    // Function to safely increment size
    auto upSize = [&size, &index, this](int plus)
    {
        if( (size+plus) <= mRxBuffer.size() )
        {
            #ifdef DEBUG_PLUS
            qDebug() << "      - (" << size << "," << index << ",+" << plus << ")";
            #endif
            size += plus;
            index = size-1;
            return true;
        }
        else
        {
            #ifdef DEBUG_PLUS
            qDebug() << "      - No more data to read need(" << size+plus << ") left(" << mRxBuffer.size() << ")";
            #endif
            return false;
        }
    };

    // size = 1
    // index = 0
    if( (uint8_t)mRxBuffer[index] != (uint8_t)0xFF )
    {
        #ifdef DEBUG_PLUS
        qDebug() << "      - Packet byte 0 must be 0xFF";
        #endif
        mRxBuffer.remove(0, size);
        return;
    }

    if( !upSize(1) ) { return ; }

    // size = 2
    // index = 1
    if( (uint8_t)mRxBuffer[index] != (uint8_t)0xFF )
    {
        #ifdef DEBUG_PLUS
        qDebug() << "      - Packet byte 1 must be 0xFF";
        #endif
        mRxBuffer.remove(0, size);
        return;
    }

    if( !upSize(1) ) { return ; }

    // size = 3
    // index = 2
    if( (uint8_t)mRxBuffer[index] != (uint8_t)0xFD )
    {
        #ifdef DEBUG_PLUS
        qDebug() << "      - Packet byte 2 must be 0xFD";
        #endif
        mRxBuffer.remove(0, size);
        return;
    }

    if( !upSize(3) ) { return ; }

    // size = 6
    // index = 5
    uint8_t lenL = mRxBuffer[index]; // 5

    if( !upSize(1) ) { return ; }
    
    // size = 7
    // index = 6
    uint8_t lenH = mRxBuffer[index]; // 6
    int len = Packet::MakeWord(lenL, lenH);

    #ifdef DEBUG_PLUS
    qDebug() << "      - Packet len(" << len << ")";
    #endif

    if(!upSize(len)) { return ; }

    #ifdef DEBUG_PLUS
    qDebug() << "      - Packet complete size(" << size << ")";
    #endif

    // Use packet
    QByteArray pbuffer = mRxBuffer.left(size);
    Packet pack(pbuffer);
    processPacket(pack);

    // Remove used data
    mRxBuffer.remove(0, size);

    // Try to parse an other packet
    parsePacket();
}

/* ============================================================================
 *
 * */
void Service::processPacket(const Packet& pack)
{
    #ifdef DEBUG_PLUS
    qDebug() << "  + Service::processPacket(" << pack.toString() << ")";
    #endif

    switch(mCurrentCommand.getType())
    {
        case Command::Type::none: break;
        case Command::Type::ping: processPacket_Ping(pack); break;
        case Command::Type::pull: processPacket_Pull(pack); break;
        case Command::Type::push: processPacket_Push(pack); break;
    }
}

/* ============================================================================
 *
 * */
void Service::processPacket_Ping(const Packet& pack)
{
    switch(pack.getInstruction())
    {
        case Packet::Instruction::InsPing:
        {
            #ifdef DEBUG_PLUS
            qDebug() << "      - Ping echo";
            #endif
            break;
        }
        case Packet::Instruction::InsStatus:
        {
            // each packet received is a servo that give its ID
            uint8_t id = pack.getId();
            mPingResult << id;
            
            #ifdef DEBUG_PLUS
            qDebug() << "      - Ping new id detected (" << id << ")";
            #endif

            emit newPingIdReceived(id);
            break;
        }
        default:
        {
            qWarning("      - Unexpected message received in ping context %s", pack.toString().toStdString().c_str());
            break;
        }
    }
}

/* ============================================================================
 *
 * */
void Service::processPacket_Pull(const Packet& pack)
{
    switch(pack.getInstruction())
    {
        case Packet::Instruction::InsRead:
        {
            #ifdef DEBUG_PLUS
            qDebug() << "      - Read echo";
            #endif
            break;
        }
        case Packet::Instruction::InsStatus:
        {
            uint8_t id = pack.getId();
            Servo* servo = getServo(id);
            servo->update((Servo::RegisterIndex)mCurrentCommand.getAddr(), pack.getReadData());
            endCommand();
            break;
        }
        default:
        {
            qWarning("      - Unexpected message received in ping context %s", pack.toString().toStdString().c_str());
            break;
        }
    }
}

/* ============================================================================
 *
 * */
void Service::processPacket_Push(const Packet& pack)
{

}

/* ============================================================================
 *
 * */
void Service::endCommand()
{
    mIsRunning = false;
    mTimerOut.stop();

    // Signal to listeners
    emit commandEnded();

    // Try to send next command
    QTimer::singleShot(0, this, SLOT(sendNextCommand()) );
}

/* ============================================================================
 *
 * */
void Service::sendNextCommand()
{
    #ifdef DEBUG_PLUS
    qDebug() << "  + Service::sendNextCommand()";
    #endif

    // No more command to send
    if( mCommandQueue.isEmpty() )
    {
        #ifdef DEBUG_PLUS
        qDebug() << "      - Command queue is empty, no more command to send";
        #endif
        return;
    }

    // A command is already running
    if( mIsRunning )
    {
        #ifdef DEBUG_PLUS
        qDebug() << "      - A command is already running, wait...";
        #endif
        return;
    }

    // Command is now running
    mIsRunning = true;
    int timeout = DefaultTimeout;

    // Take next comment
    mCurrentCommand = mCommandQueue.dequeue();

    // Reset ping result if it is an other ping command
    if(mCurrentCommand.getType() == Command::Type::ping)
    {
        timeout = PingTimeout;
        mPingResult.clear();
    }

    // Send command
    emit commandTransmissionRequested( mCurrentCommand.toDataArray() );

    #ifdef DEBUG_PLUS
    qDebug() << "      - command sent(" << mCurrentCommand.toDataArray() << ")";
    #endif

    // Start timeout timer
    mTimerOut.start(timeout);
}

/* ============================================================================
 *
 * */
void Service::manageCommandTimeout()
{
    endCommand();
}

/* ============================================================================
 *
 * */
void Service::receiveData(const QByteArray& data)
{
    #ifdef DEBUG_PLUS
    qDebug() << "  + Service::receiveData(" << data << ")";
    #endif

    // First store data
    mRxBuffer += data;

    //! Try to get packet from it
    parsePacket();
}

/* ============================================================================
 *
 * */
void Service::sendPing()
{
    registerCommand( Command(Command::Type::ping) );
}
