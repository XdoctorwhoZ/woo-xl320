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
void Service::parsePacket()
{
    #ifdef DEBUG_PLUS
    qDebug() << "  + Service::parsePacket()";
    #endif

    // Check if the buffre is not empty
    if (mRxBuffer.isEmpty())
    {
        #ifdef DEBUG_PLUS
        qDebug() << "      - Rx buffer is empty";
        #endif
        return;
    }

    // Initialize packet size
    uint32_t size = 0;

    // Function to safely increment size
    auto upSize = [&size, this](int plus)
    {
        if( (size+plus) <= mRxBuffer.size() )
        {
            size += plus;
            return true;
        }
        else { return false; }
    };

    // Check packet
    if( (uint8_t)mRxBuffer[size] != (uint8_t)0xFF )
    {
        #ifdef DEBUG_PLUS
        qDebug() << "      - Packet byte 0 must be 0xFF";
        #endif
        mRxBuffer.remove(0, size+1);
        return;
    }
    if(!upSize(1)) { return ; }
    if( (uint8_t)mRxBuffer[size] != (uint8_t)0xFF )
    {
        #ifdef DEBUG_PLUS
        qDebug() << "      - Packet byte 1 must be 0xFF";
        #endif
        mRxBuffer.remove(0, size+1);
        return;
    }
    if(!upSize(1)) { return ; }
    if( (uint8_t)mRxBuffer[size] != (uint8_t)0xFD )
    {
        #ifdef DEBUG_PLUS
        qDebug() << "      - Packet byte 2 must be 0xFD";
        #endif
        mRxBuffer.remove(0, size+1);
        return;
    }
    if(!upSize(3)) { return ; }
    uint8_t lenL = mRxBuffer[size]; // 5
    if(!upSize(1)) { return ; }
    uint8_t lenH = mRxBuffer[size]; // 6
    if(!upSize(1)) { return ; }
    int len = Packet::MakeWord(lenL, lenH);
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
        case Command::Type::none:
        {

            break;
        }
        case Command::Type::ping: processPacket_Ping(pack); break;
        case Command::Type::pull:
        {

            break;
        }
        case Command::Type::push:
        {

            break;
        }
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

    // Take next comment
    mCurrentCommand = mCommandQueue.dequeue();

    // Reset ping result if it is an other ping command
    if(mCurrentCommand.getType() == Command::Type::ping)
    {
        mPingResult.clear();
    }

    // Send command
    emit commandTransmissionRequested( mCurrentCommand.toDataArray() );

    #ifdef DEBUG_PLUS
    qDebug() << "      - command sent(" << mCurrentCommand.toDataArray() << ")";
    #endif

    // Start timeout timer
    // mTimerOut.start(CommandTimeout);

}

/* ============================================================================
 *
 * */
void Service::receiveData(const QByteArray& data)
{
    #ifdef DEBUG_PLUS
    qDebug() << "  + Service::parseData(" << data << ")";
    #endif

    // First store data
    mRxBuffer += data;

    //! Try to get packet from it
    parsePacket();
}

/* ============================================================================
 *
 * */
// void Service::sendTest()
// {
//     // registerCommand();
// }

/* ============================================================================
 *
 * */
void Service::sendPing()
{
    registerCommand( Command(Command::Type::ping) );
}
