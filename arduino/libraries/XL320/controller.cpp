// Woo
#include "XL320.h"

// ---
using namespace xl320;

// Uncomment to debug
#define XL320Controller_DEBUG

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
void Controller::orderExec(Order* order)
{
    // Fill buffer
    const byte size = order->getRequiredSize();
    byte buffer[size];
    order->fillBuffer(buffer, size);

    // Debug
    #ifdef XL320Controller_DEBUG
    Serial.print("+ Order={");
    for(int i=0 ; i<size ; i++)
    {
        if(i!=0) { Serial.print(", "); }
        Serial.print((int)buffer[i]);
    }
    Serial.println("}");
    #endif

    // Send packet
    mXlSerial->write(buffer,size);
    mXlSerial->flush();
}

/* ============================================================================
 *
 * */
void Controller::selectServo(const byte* ids, byte number)
{
    mSelectSize = number;
    for(int i=0 ; i<mSelectSize ; i++)
    {
        mSelectIds[i] = ids[i];
    }
}

/* ============================================================================
 *
 * */
int Controller::receiveData(unsigned long usTimeout)
{
    // Reset
    mRxBufferSize = 0;
    mRxPtr = mRxBuffer;

    int rsize = 0;
    unsigned long startTime = micros();

    #ifdef XL320Controller_DEBUG
    Serial.print("+ ReceiveData:t=");
    Serial.print(usTimeout, DEC);
    Serial.print("us... ");
    #endif

    // While not timeout and buffer not full
    while( (abs(micros() - startTime) <= usTimeout) && (mRxBufferSize < RxBufferSizeMax) )
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
    Serial.print("+ Number of data received:");
    Serial.println(mRxBufferSize, DEC);
    #endif

    return mRxBufferSize;
}

/* ============================================================================
 *
 * */
int Controller::getNextPacket(Packet& pack)
{
    byte size = 0;
    byte* packStart = mRxPtr;
    int left = (int)(mRxBufferSize-(mRxPtr-mRxBuffer));

    #ifdef XL320Controller_DEBUG
    Serial.println("+ Next Packet");
    Serial.print("    - left=");
    Serial.println(left, DEC);
    #endif // XL320Controller_DEBUG

    // Check if there are some data left
    if(left <= 0) { return -42; }

    // Function to safely increment size
    auto upSize = [&size, left, this](int plus)
    {
        if( (size+plus) <= left ) {
            size += plus;
            return true;
        }
        else { return false; }
    };

    // Check packet
    if( mRxPtr[size] != (byte)0xFF ) {
        mRxPtr += (size+1); return -1; // 0
    }
    if(!upSize(1)) { return -42; }
    if( mRxPtr[size] != (byte)0xFF ) {
        mRxPtr += (size+1); return -2; // 1
    }
    if(!upSize(1)) { return -42; }
    if( mRxPtr[size] != (byte)0xFD ) {
        mRxPtr += (size+1); return -2; // 2
    }
    if(!upSize(3)) { return -42; }
    byte lenL = mRxPtr[size]; // 5
    if(!upSize(1)) { return -42; }
    byte lenH = mRxPtr[size]; // 6
    if(!upSize(1)) { return -42; }
    int len = DXL_MAKEWORD(lenL, lenH);
    if(!upSize(len)) { return -42; }

    // Prepare return value
    mRxPtr += size;
    pack = Packet(packStart, size);

    #ifdef XL320Controller_DEBUG
    Serial.print("    - New packet:size=");
    Serial.print(size, DEC);
    Serial.print(", data=");
    Serial.println(pack.toString().c_str());
    #endif

    return 0;
}

/* ============================================================================
 *
 * */
int Controller::readValuesFromRxPackets(int* values)
{
    // Parse each packet
    Packet pack;
    int sts;
    int number = mSelectSize;
    while( (sts=getNextPacket(pack)) != -42 )
    {
        if(sts != 0) 
        {
            #ifdef XL320Controller_DEBUG
            Serial.print("    - Echo instruction ");
            Serial.print((int)pack.getInstruction());
            #endif
            continue;
        }
        // Check that it is a status packet
        if(pack.getInstruction() == InsStatus)
        {
            int value = 0;

            const int pcount = pack.getParameterCount();
            switch(pcount)
            {
                case 2:
                    // 0 -> error reg
                    value = (int) pack.getParameter(1);
                    break;
                case 3:
                    // 0 -> error reg
                    value = (int) DXL_MAKEWORD(pack.getParameter(1), pack.getParameter(2));
                    break;
                default:
                    value = 0;
            }

            const int id = pack.getId();

            #ifdef XL320Controller_DEBUG
            Serial.print("    - Status from ");
            Serial.print(id);
            Serial.print(" val=");
            Serial.println(value);
            #endif

            for(int i=0 ; i<mSelectSize ; i++)
            {
                if( (int)mSelectIds[i] == id )
                {
                    values[i] = value;
                }
            }

            number--;
        }
        // else drop packet (it is our TX)
    }
    return number;
}

/* ============================================================================
 *
 * */
byte Controller::ping(byte* ids)
{
    // Prepare order
    SimpleOrder order;
    order.type = Order::Type::Ping;
    order.id   = Constant::BroadcastId;
    orderExec(&order);

    // Read data from serial
    receiveData(RxBaseTimeout);

    // Parse each packet
    Packet pack;
    int sts;
    int number = 0;
    while( (sts=getNextPacket(pack)) != -42 )
    {
        if(sts != 0) continue;
        // Check that it is a status packet
        if(pack.getInstruction() == InsStatus)
        {
            ids[number] = (byte)pack.getId();
            number++;
        }
        // else drop packet (it is our TX)
    }
    return number;
}

/* ============================================================================
 *
 * */
byte Controller::pull(RegIndex index, int* values)
{
    // Prepare order
    MultiOrder order;
    order.type = Order::Type::Pull;
    order.idsSize = mSelectSize;
    for(int i=0 ; i<mSelectSize ; i++)
    {
        order.ids[i] = mSelectIds[i];
    }
    order.index = index;
    orderExec(&order);

    // Read data from serial
    receiveData(RxBaseTimeout);

    // Parse incomming data
    return readValuesFromRxPackets(values);
}
