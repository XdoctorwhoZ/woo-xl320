// Woo
#include "XL320Controller.hpp"

// ---
using namespace xl320;

// Uncomment to debug
#define XL320Controller_DEBUG

/* ============================================================================
 *
 * */
const ControlReg Controller::ControlTable[CiTotal] =
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
    : mNumberOfSelectedServo(1)
{
    mSelectedServoIds[0] = 1;
}

/* ============================================================================
 *
 * */
Controller::~Controller()
{ }

/* ============================================================================
 *
 * */
void Controller::setup(XlSerial& xlSerial)
{
    mXlSerial = &xlSerial;
    setXlBaudRate(BaudRate::Br115200);
}

/* ============================================================================
 *
 * */
BaudRate Controller::getXlBaudRate() const
{
    return mXlSerialBaudRate;
}

/* ============================================================================
 *
 * */
String Controller::getXlBaudRateString() const
{
    switch( mXlSerialBaudRate )
    {
        case Br9600     : return String("9600"   );
        case Br57600    : return String("57600"  );
        case Br115200   : return String("115200" );
        case Br1Mbps    : return String("1000000");
        default         : return String("?");
    }
}

/* ============================================================================
 *
 * */
void Controller::setXlBaudRate(BaudRate br)
{
    mXlSerialBaudRate = br;
    // Modify serial config
    mXlSerial->end();
    switch(mXlSerialBaudRate)
    {
        case Br9600   : mXlSerial->begin(9600);    break;
        case Br57600  : mXlSerial->begin(57600);   break;
        case Br115200 : mXlSerial->begin(115200);  break;
        case Br1Mbps  : mXlSerial->begin(1000000); break;
    }
}

/* ============================================================================
 *
 * */
String Controller::getSelectedServoString() const
{
    String list = "";
    for(int i=0 ; i<mNumberOfSelectedServo ; i++)
    {
        if(i == 0)
        {
            list += (int)mSelectedServoIds[i];
        }
        else
        {
            list += ',';
            list += (int)mSelectedServoIds[i];
        }
    }
    return list;
}

/* ============================================================================
 *
 * */
void Controller::selectServo(const byte* ids, byte number)
{
    mNumberOfSelectedServo = number;
    for(int i=0 ; i<mNumberOfSelectedServo ; i++)
    {
        mSelectedServoIds[i] = ids[i];
    }
}

/* ============================================================================
 *
 * */
int Controller::readNextPacket(byte* buffer, int msize)
{
    int size = 0;

    while (mXlSerial->available())
    {
        char c = mXlSerial->read();
        Serial.println((int) c, HEX);

    }

    return -1;
}

/* ============================================================================
 *
 * */
void Controller::sendPingPacket()
{
    const int params_size = 0;
    const int bsize = Packet::ComputeBufferSize(params_size);

    // buffers
    byte buffer[bsize];

    // Build packet
    Packet pack(buffer, bsize);
    pack.build(Constant::BroadcastId, InsPing, params_size);

    // Send packet
    mXlSerial->write(buffer,bsize);
    mXlSerial->flush();
}

/* ============================================================================
 *
 * */
void Controller::sendWritePacket(byte id, ControlIndex ci, int value) const
{
    // addr = 2 + sizeof(reg)
    const int params_size = 2 + ControlTable[ci].size;
    const int bsize = Packet::ComputeBufferSize(params_size);

    // buffers
    byte buffer[bsize];
    byte params[params_size];

    // params
    params[0] = DXL_LOBYTE(ControlTable[ci].addr);
    params[1] = DXL_HIBYTE(ControlTable[ci].addr);
    if( ControlTable[ci].size == 1 )
    {
        params[2] = DXL_LOBYTE(value);
    }
    else if( ControlTable[ci].size == 2 )
    {
        params[2] = DXL_LOBYTE(value);
        params[3] = DXL_HIBYTE(value);
    }

    // Build packet
    Packet pack(buffer, bsize);
    pack.build(id, InsWrite, params_size, params);

    // Send packet
    mXlSerial->write(buffer,bsize);
    mXlSerial->flush();
}

/* ============================================================================
 *
 * */
int Controller::ping(byte* ids)
{
    int number = 0;

    sendPingPacket();


    const int rbufferSize = 32;
    byte rbuffer[rbufferSize];
    readNextPacket(rbuffer, rbufferSize);


    return number;
}

/* ============================================================================
 *
 * */
void Controller::getGpos(int* positions) const
{

}

/* ============================================================================
 *
 * */
void Controller::setGpos(const int* positions, int number) const
{

    if(mNumberOfSelectedServo == 1)
        sendWritePacket(mSelectedServoIds[0], CiGoalPosition, positions[0]);



}

/* ============================================================================
 *
 * */
void Controller::getGspeed(int* speeds) const
{

}

/* ============================================================================
 *
 * */
void Controller::setGspeed(const int* speeds) const
{

}
