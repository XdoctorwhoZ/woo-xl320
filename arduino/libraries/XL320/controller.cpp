// Woo
#include "XL320.h"

// ---
using namespace xl320;

/* ============================================================================
 *
 * */
const RegInfo Controller::RegInfoTable[RegIndex::Total] =
{
    /*EEPROM Area*/
    {  0 , 2 } , // CiModelNumber
    {  2 , 1 } , // CiVersion
    {  3 , 1 } , // CiId
    {  4 , 1 } , // CiBaudRate                
    {  5 , 1 } , // CiReturnDelayTime         
    {  6 , 2 } , // CiCwAngleLimit  
    {  8 , 2 } , // CiCcwAngleLimit
    { 11 , 1 } , // CiControlMode      
    { 12 , 1 } , // CiLimitTemperature 
    { 13 , 1 } , // CiDownLimitVoltage
    { 14 , 1 } , // CiUpLimitVoltage
    { 15 , 2 } , // CiMaxTorque
    { 17 , 1 } , // CiReturnLevel   
    { 18 , 1 } , // CiAlarmShutdown 
    /*RAM Area*/
    { 24 , 1 } , // CiTorqueEnable         
    { 25 , 2 } , // CiLed                  
    { 27 , 1 } , // CiDgain                
    { 28 , 1 } , // CiIgain                
    { 29 , 1 } , // CiPgain                
    { 30 , 2 } , // CiGoalPosition         
    { 32 , 2 } , // CiGoalSpeed            
    { 35 , 2 } , // CiGoalTorque           
    { 37 , 2 } , // CiPresentPosition      
    { 39 , 2 } , // CiPresentSpeed         
    { 41 , 2 } , // CiPresentLoad          
    { 45 , 1 } , // CiPresentVoltage       
    { 46 , 1 } , // CiPresentTemperature   
    { 47 , 1 } , // CiRegisteredInstruction
    { 49 , 1 } , // CiMoving               
    { 50 , 1 } , // CiHardwareError        
    { 51 , 2 } , // CiPunch                
};

/* ============================================================================
 *
 * */
Controller::Controller()
    : mSelectSize(1)
{
    mSelectIds[0] = 1;
}

/* ============================================================================
 *
 * */
void Controller::setup(XlSerial& xlSerial)
{
    mXlSerial = &xlSerial;
    setXlBaudRate(1000000);
}

/* ============================================================================
 *
 * */
void Controller::setXlBaudRate(baudrate_t br)
{
    mXlSerialBaudRate = br;
    mXlSerial->end();
    mXlSerial->begin(mXlSerialBaudRate);
}

/* ============================================================================
 *
 * */
void Controller::selectServo(const uint8_t* ids, uint8_t number)
{
    mSelectSize = number;
    for(uint8_t i=0 ; i<mSelectSize ; i++)
    {
        mSelectIds[i] = ids[i];
    }
}

/* ============================================================================
 *
 * */
// #define Controller__ping_DEBUG
uint8_t Controller::ping(uint8_t* ids)
{
    Packet pack;
    uint8_t sts;
    uint8_t number;

    #ifdef Controller__ping_DEBUG
    Serial.println("#### Controller::ping : begin");
    #endif

    // Prepare data buffer
    const uint8_t bsize = Packet::GetBsPing();
    uint8_t buffer[bsize];
    Packet::FillBPing(buffer, bsize);

    #ifdef Controller__ping_DEBUG
    Serial.print("#### Controller::ping : data prepared size = ");
    Serial.println(bsize);
    #endif

    // Send packet
    sendBuffer(buffer, bsize);

    #ifdef Controller__ping_DEBUG
    Serial.println("#### Controller::ping : data sent, wait for an answer");
    #endif

    // Read data from serial
    receiveData(RxBaseTimeout);

    #ifdef Controller__ping_DEBUG
    Serial.print("#### Controller::ping : ");
    Serial.print(mRxBufferSize);
    Serial.println(" bytes received");
    #endif

    // Parse each packet
    number = 0;
    while( (sts=getNextPacket(pack)) != EndOfBuffer )
    {
        #ifdef Controller__ping_DEBUG
        Serial.print("#### Controller::ping : analyse packet=");
        Serial.println(pack.toString().c_str());
        #endif

        if(sts != ValidPacket)
        {
            #ifdef Controller__ping_DEBUG
            Serial.println("#### Controller::ping : incomplet packet");
            #endif
            continue;
        }

        // Check that it is a status packet
        if( pack.isInstructionStatus() )
        {
            ids[number] = pack.getId();

            #ifdef Controller__ping_DEBUG
            Serial.print("#### Controller::ping : new id ");
            Serial.println((int)ids[number]);
            #endif

            number++;
        }
        #ifdef Controller__ping_DEBUG
        else
        {
            Serial.print("#### Controller::ping : echo ");
            Serial.println( (int)pack.getInstruction() );
        }
        #endif
    }

    #ifdef Controller__ping_DEBUG
    Serial.print("#### Controller::ping : end ");
    Serial.print(number);
    Serial.println(" servos found");
    #endif

    return number;
}

/* ============================================================================
 *
 * */
// #define Controller__pull_DEBUG
uint8_t Controller::pull(RegIndex index, uint16_t* returnVector)
{
    Packet pack;
    uint8_t sts;
    uint8_t number;
    uint16_t value;

    #ifdef Controller__pull_DEBUG
    Serial.println("==== Controller::pull : begin");
    #endif

    // Prepare data buffer
    const uint8_t bsize = Packet::GetBsPull(mSelectSize);
    uint8_t buffer[bsize];
    Packet::FillBPull(buffer, bsize
                            , mSelectIds
                            , mSelectSize
                            , RegInfoTable[index].addr
                            , RegInfoTable[index].size
                            );

    #ifdef Controller__pull_DEBUG
    Serial.print("==== Controller::pull : data prepared size = ");
    Serial.println(bsize);
    #endif

    // Read data from serial
    receiveData(RxBaseTimeout);

    #ifdef Controller__pull_DEBUG
    Serial.print("==== Controller::pull : ");
    Serial.print(mRxBufferSize);
    Serial.println(" bytes received");
    #endif

    // Parse each packet
    number = 0;
    while( (sts=getNextPacket(pack)) != EndOfBuffer )
    {
        #ifdef Controller__pull_DEBUG
        Serial.print("==== Controller::pull : analyse packet=");
        Serial.println(pack.toString().c_str());
        #endif

        if(sts != ValidPacket)
        {
            #ifdef Controller__pull_DEBUG
            Serial.println("==== Controller::pull : incomplet packet");
            #endif
            continue;
        }

        // Check that it is a status packet
        if( pack.isInstructionStatus() )
        {
            const uint8_t id = pack.getId();

            #ifdef Controller__pull_DEBUG
            Serial.print("==== Controller::pull : answer from id ");
            Serial.println(id);
            #endif

            const uint8_t pcount = pack.getParameterCount();
            switch(pcount)
            {
                case 2:
                    value = (uint16_t) pack.getParameter(1);
                    break;
                case 3:
                    value = (uint16_t) Packet::MakeWord(pack.getParameter(1), pack.getParameter(2));
                    break;
                default:
                    value = 100;
                    break;
            }

            #ifdef Controller__pull_DEBUG
            Serial.print("==== Controller::pull : value is ");
            Serial.println((int)value);
            #endif

            for(uint8_t i=0 ; i<mSelectSize ; i++)
            {
                if( mSelectIds[i] == id )
                {
                    #ifdef Controller__pull_DEBUG
                    Serial.print("==== Controller::pull : index ");
                    Serial.println((int)i);
                    #endif

                    returnVector[i] = value;
                    break;
                }
            }

            number++;
        }
        #ifdef Controller__pull_DEBUG
        else
        {
            Serial.print("==== Controller::pull : echo ");
            Serial.println( (int)pack.getInstruction() );
        }
        #endif
    }

    #ifdef Controller__ping_DEBUG
    Serial.print("==== Controller::pull : end ");
    Serial.print(number);
    Serial.println(" answers");
    #endif

    return number;
}

/* ============================================================================
 *
 * */
// #define Controller__push_DEBUG
void Controller::push(RegIndex index, uint16_t* values)
{
    #ifdef Controller__push_DEBUG
    Serial.println("==== Controller::push : begin");
    #endif

    // Prepare data buffer
    const uint8_t bsize = Packet::GetBsPush(mSelectSize, RegInfoTable[index].size);
    uint8_t buffer[bsize];
    Packet::FillBPush(buffer, bsize
                            , mSelectIds
                            , mSelectSize
                            , RegInfoTable[index].addr
                            , RegInfoTable[index].size
                            , values
                            );

    #ifdef Controller__push_DEBUG
    Serial.print("==== Controller::pull : data prepared size = ");
    Serial.println(bsize);
    #endif

    // Read data from serial
    receiveData(RxBaseTimeout);
    dropRxData();

    #ifdef Controller__push_DEBUG
    Serial.println("==== Controller::push : end");
    #endif
}

/* ============================================================================
 *
 * */
// #define Controller__reset_DEBUG
void Controller::reset()
{
    #ifdef Controller__reset_DEBUG
    Serial.println("#### Controller::reset : begin");
    #endif

    // Prepare data buffer
    const uint8_t bsize = Packet::GetBsReset();
    uint8_t buffer[bsize];
    Packet::FillBReset(buffer, bsize);

    // Send packet
    sendBuffer(buffer, bsize);

    // Read data from serial
    receiveData(RxBaseTimeout);
    dropRxData();

    #ifdef Controller__reset_DEBUG
    Serial.println("#### Controller::reset : end");
    #endif
}

/* ============================================================================
 *
 * */
// #define Controller__reboot_DEBUG
void Controller::reboot()
{
    #ifdef Controller__reboot_DEBUG
    Serial.println("#### Controller::reboot : begin");
    #endif

    // Prepare data buffer
    const uint8_t bsize = Packet::GetBsReboot();
    uint8_t buffer[bsize];
    Packet::FillBReboot(buffer, bsize);

    // Send packet
    sendBuffer(buffer, bsize);

    // Read data from serial
    receiveData(RxBaseTimeout);
    dropRxData();

    #ifdef Controller__reboot_DEBUG
    Serial.println("#### Controller::reboot : end");
    #endif
}

/* ============================================================================
 *
 * */
// #define Controller__sendBuffer_DEBUG
void Controller::sendBuffer(uint8_t* buffer, uint8_t bsize)
{
    #ifdef Controller__sendBuffer_DEBUG
    Serial.print("send={");
    for(uint8_t i=0 ; i<bsize ; i++)
    {
        if(i!=0) { Serial.print(", "); }
        Serial.print((int)buffer[i], HEX);
    }
    Serial.println("}");
    #endif

    // Send packet
    mXlSerial->write(buffer,bsize);
    mXlSerial->flush();
}

/* ============================================================================
 *
 * */
uint16_t Controller::receiveData(uint16_t msTimeout)
{
    // Reset
    dropRxData();

    int rsize = 0;
    unsigned long startTime = millis();

    #ifdef XL320Controller_DEBUG
    Serial.print("    + ReceiveData:t=");
    Serial.print(msTimeout, DEC);
    Serial.println("us... ");
    #endif

    // While not timeout and buffer not full
    while( (abs(millis() - startTime) <= msTimeout) && (mRxBufferSize < RxBufferSizeMax) )
    {
        while( (rsize = mXlSerial->available()) > 0 )
        {
            // Check that rx buffer is not full
            if( (mRxBufferSize + rsize) >= RxBufferSizeMax )
            {
                rsize = RxBufferSizeMax - mRxBufferSize;
            }

            // Read those bytes
            mRxBufferSize += mXlSerial->readBytes(mRxBuffer + mRxBufferSize, rsize);
        }
    }

    #ifdef XL320Controller_DEBUG
    Serial.print("    + Number of data received:");
    Serial.println(mRxBufferSize, DEC);
    #endif

    return mRxBufferSize;
}

/* ============================================================================
 *
 * */
// #define Controller__getNextPacket_DEBUG
uint8_t Controller::getNextPacket(Packet& pack)
{
    uint8_t  size = 0;
    uint8_t* packStart = mRxPtr;
    uint16_t left = (int)(mRxBufferSize-(mRxPtr-mRxBuffer));

    #ifdef Controller__getNextPacket_DEBUG
    Serial.print("**** Next packet : left=");
    Serial.println(left, DEC);
    #endif

    // Check if there are some data left
    if(left <= 0) { return EndOfBuffer; }

    // Function to safely increment size
    auto upSize = [&size, left, this](uint16_t plus)
    {
        if( (size+plus) <= left )
        {
            size += plus;
            return true;
        }
        else
        {
            return false;
        }
    };

    // Check packet
    if( mRxPtr[size] != (uint8_t)0xFF ) {
        mRxPtr += (size+1); return IncompletPk; // 0
    }
    if(!upSize(1)) { return EndOfBuffer; }
    if( mRxPtr[size] != (uint8_t)0xFF ) {
        mRxPtr += (size+1); return IncompletPk; // 1
    }
    if(!upSize(1)) { return EndOfBuffer; }
    if( mRxPtr[size] != (uint8_t)0xFD ) {
        mRxPtr += (size+1); return IncompletPk; // 2
    }
    if(!upSize(3)) { return EndOfBuffer; }
    uint8_t lenL = mRxPtr[size]; // 5
    if(!upSize(1)) { return EndOfBuffer; }
    uint8_t lenH = mRxPtr[size]; // 6
    if(!upSize(1)) { return EndOfBuffer; }
    uint16_t len = Packet::MakeWord(lenL, lenH);
    if(!upSize(len)) { return EndOfBuffer; }

    // Prepare return value
    mRxPtr += size;
    pack = Packet(packStart, size);

    #ifdef Controller__getNextPacket_DEBUG
    Serial.print("**** Next packet : complete size=");
    Serial.print(size, DEC);
    Serial.print(", data=");
    Serial.println(pack.toString().c_str());
    #endif

    return 0;
}

/* ============================================================================
 *
 * */
void Controller::dropRxData()
{
    mRxBufferSize = 0;
    mRxPtr = mRxBuffer;
}
