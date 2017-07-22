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
String Controller::ErrorToString(const ErrorData& err)
{
    String str(err.id);
    switch(err.data)
    {
        case EnResultFail       : str += "-Result Fail";         break;
        case EnInstructionError : str += "-Instruction Error";   break;
        case EnCRCError         : str += "-CRC Error";           break;
        case EnDataRangeError   : str += "-Data Range Error";    break;
        case EnDataLengthError  : str += "-Data Length Error";   break;
        case EnDataLimitError   : str += "-Data Limit Error";    break;
        case EnAccessError      : str += "-Access Error";        break;
    }
    return str;
}

/* ============================================================================
 *
 * */
int Controller::receiveData(unsigned long usTimeout)
{
    // Reset
    mRxBytes = 0;
    mRxPtr = mRxBuffer;

    int rsize = 0;
    unsigned long startTime = micros();

#ifdef XL320Controller_DEBUG
    Serial.print("#ReceiveData:t=");
    Serial.print(usTimeout, DEC);
    Serial.print("us... ");
#endif // XL320Controller_DEBUG

    // While not timeout and buffer not full
    while( (abs(micros() - startTime) <= usTimeout) && (mRxBytes < RxBufferSize) )
    {
        while( (rsize = mXlSerial->available()) > 0 )
        {
            // Check that rx buffer is not full
            if( (mRxBytes + rsize) >= RxBufferSize )
            {
                rsize = RxBufferSize - mRxBytes;
            }

            // Read those bytes
            mRxBytes += mXlSerial->readBytes(mRxBuffer + mRxBytes, rsize);
        }
    }

#ifdef XL320Controller_DEBUG
    Serial.print("size=");
    Serial.println(mRxBytes, DEC);
#endif // XL320Controller_DEBUG

    return mRxBytes;
}

/* ============================================================================
 *
 * */
int Controller::getNextPacket(Packet& pack)
{
    byte size = 0;
    byte* packStart = mRxPtr;
    int left = (int)(mRxBytes-(mRxPtr-mRxBuffer));

#ifdef XL320Controller_DEBUG
    Serial.print("#RxBuffer:left=");
    Serial.println(left, DEC);
#endif // XL320Controller_DEBUG

    // Check if there are some data left
    if(left <= 0) { return RxBufferEmpty; }

    // Function to safely increment size
    auto upSize = [&size, this](int plus)
    {
        if( (size+plus) <= this->mRxBytes ) { size += plus; return true; }
        else { return false; }
    };

    // Check packet
    if( mRxPtr[size] != (byte)0xFF ) return -1; // 0
    if(!upSize(1)) { return RxBufferEmpty; }
    if( mRxPtr[size] != (byte)0xFF ) return -2; // 1
    if(!upSize(1)) { return RxBufferEmpty; }
    if( mRxPtr[size] != (byte)0xFD ) return -2; // 2
    if(!upSize(3)) { return RxBufferEmpty; }
    byte lenL = mRxPtr[size]; // 5
    if(!upSize(1)) { return RxBufferEmpty; }
    byte lenH = mRxPtr[size]; // 6
    if(!upSize(1)) { return RxBufferEmpty; }
    int len = DXL_MAKEWORD(lenL, lenH);
    if(!upSize(len)) { return RxBufferEmpty; }

    // Prepare return value
    mRxPtr += size;
    pack = Packet(packStart, size);

#ifdef XL320Controller_DEBUG
    Serial.print("#New packet:size=");
    Serial.print(size, DEC);
    Serial.print(", data=");
    Serial.println(pack.toString().c_str());
#endif // XL320Controller_DEBUG

    return 0;
}

/* ============================================================================
 *
 * */
int Controller::readValuesFromRxPackets(int* values)
{
    // Read data from serial 50ms
    receiveData(mNumberOfSelectedServo*1000);

    // Parse each packet
    Packet pack;
    int sts;
    int number = mNumberOfSelectedServo;
    while( (sts=getNextPacket(pack)) != RxBufferEmpty )
    {
        if(sts != 0) continue;
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
            for(int i=0 ; i<mNumberOfSelectedServo ; i++)
            {
                if( (int)mSelectedServoIds[i] == id )
                {
                    values[i] = value;
                }
            }

            number--;
        }
        // else drop packet (it is our TX)
    }
    return number;


    // // RX buffer
    // const int rxBufferSize = 32;
    // byte rxBuffer[rxBufferSize];

    // // Check rx buffer
    // int size = 0;
    // int number = mNumberOfSelectedServo;
    // while(size != -42)
    // {
    //     // Read one packet only
    //     size = readNextPacket(rxBuffer, rxBufferSize);
    //     if(size == -42) {
    //         break;
    //     }

    //     // Prepare packet parsing
    //     Packet pack(rxBuffer, size);

    //     // If the packet is an return status only (else it is the out Tx...)
    //     if(pack.getInstruction() == InsStatus)
    //     {
    //         int value = 0;

    //         const int pcount = pack.getParameterCount();

    //         switch(pcount)
    //         {
    //             case 2:
    //                 // 0 -> error reg
    //                 value = (int) pack.getParameter(1);
    //                 break;
    //             case 3:
    //                 // 0 -> error reg
    //                 value = (int) DXL_MAKEWORD(pack.getParameter(1), pack.getParameter(2));
    //                 break;
    //             default:
    //                 value = 0;
    //         }

    //         const int id = pack.getId();
    //         for(int i=0 ; i<mNumberOfSelectedServo ; i++)
    //         {
    //             if( (int)mSelectedServoIds[i] == id )
    //             {
    //                 values[i] = value;
    //             }
    //         }

    //         number--;
    //     }
    // }

    // return number;
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

#ifdef XL320Controller_DEBUG
    Serial.print("#SendPing:size=");
    Serial.print(bsize, DEC);
    Serial.print(", data=");
    Serial.println(pack.toString().c_str());
#endif // XL320Controller_DEBUG

    // Send packet
    mXlSerial->write(buffer,bsize);
    mXlSerial->flush();
}

/* ============================================================================
 *
 * */
void Controller::sendReadPacket(byte id, ControlIndex ci) const
{
    // addr = 2 + len = 2
    const int params_size = 4;
    const int bsize = Packet::ComputeBufferSize(params_size);

    // buffers
    byte buffer[bsize];
    byte params[params_size];

    // params
    params[0] = DXL_LOBYTE(ControlTable[ci].addr);
    params[1] = DXL_HIBYTE(ControlTable[ci].addr);
    params[2] = DXL_LOBYTE(ControlTable[ci].size);
    params[3] = DXL_HIBYTE(ControlTable[ci].size);

    // Build packet
    Packet pack(buffer, bsize);
    pack.build(id, InsRead, params_size, params);

#ifdef XL320Controller_DEBUG
    Serial.print("#SendReadPack:size=");
    Serial.print(bsize, DEC);
    Serial.print(", data=");
    Serial.println(pack.toString().c_str());
#endif // XL320Controller_DEBUG

    // Send packet
    mXlSerial->write(buffer,bsize);
    mXlSerial->flush();
}

/* ============================================================================
 *
 * */
void Controller::sendSyncReadPacket(ControlIndex ci) const
{
    // addr = 2 + len = 2
    const int params_size = 4 + mNumberOfSelectedServo;
    const int bsize = Packet::ComputeBufferSize(params_size);

    // buffers
    byte buffer[bsize];
    byte params[params_size];

    // params
    params[0] = DXL_LOBYTE(ControlTable[ci].addr);
    params[1] = DXL_HIBYTE(ControlTable[ci].addr);
    params[2] = DXL_LOBYTE(ControlTable[ci].size);
    params[3] = DXL_HIBYTE(ControlTable[ci].size);
    for(int i=0 ; i<mNumberOfSelectedServo ; i++)
    {
        params[3+i] = mSelectedServoIds[i];
    }

    // Build packet
    Packet pack(buffer, bsize);
    pack.build(Constant::BroadcastId, InsSyncRead, params_size, params);

#ifdef XL320Controller_DEBUG
    Serial.print("#SendSyncReadPack:size=");
    Serial.print(bsize, DEC);
    Serial.print(", data=");
    Serial.println(pack.toString().c_str());
#endif // XL320Controller_DEBUG

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

#ifdef XL320Controller_DEBUG
    Serial.print("#SendWritePack:size=");
    Serial.print(bsize, DEC);
    Serial.print(", data=");
    Serial.println(pack.toString().c_str());
#endif // XL320Controller_DEBUG

    // Send packet
    mXlSerial->write(buffer,bsize);
    mXlSerial->flush();
}

/* ============================================================================
 *
 * */
void Controller::sendSyncWritePacket(ControlIndex ci, const int* values, int num) const
{
    // addr = 2 + size = 2 + n*(1+sizeof(data))
    const int params_size = 4 + ( num * ( 1 + ControlTable[ci].size ) );
    const int bsize = Packet::ComputeBufferSize(params_size);

    // buffers
    byte buffer[bsize];
    byte params[params_size];

    // params
    params[0] = DXL_LOBYTE(ControlTable[ci].addr);
    params[1] = DXL_HIBYTE(ControlTable[ci].addr);
    params[2] = DXL_LOBYTE(ControlTable[ci].size);
    params[3] = DXL_HIBYTE(ControlTable[ci].size);
    int shift = 0;
    for(int v=0 ; v<num ; v++)
    {
        params[4 + shift + 0] = (byte) mSelectedServoIds[v];
        if( ControlTable[ci].size == 1 )
        {
            params[4 + shift + 1] = DXL_LOBYTE(values[v]);
            shift += 2;
        }
        else if( ControlTable[ci].size == 2 )
        {
            params[4 + shift + 1] = DXL_LOBYTE(values[v]);
            params[4 + shift + 2] = DXL_HIBYTE(values[v]);
            shift += 3;
        }
    }

    // Build packet
    Packet pack(buffer, bsize);
    pack.build(Constant::BroadcastId, InsSyncWrite, params_size, params);

#ifdef XL320Controller_DEBUG
    Serial.print("#SendSyncWritePack:size=");
    Serial.print(bsize, DEC);
    Serial.print(", data=");
    Serial.println(pack.toString().c_str());
#endif // XL320Controller_DEBUG

    // Send packet
    mXlSerial->write(buffer,bsize);
    mXlSerial->flush();
}

/* ============================================================================
 *
 * */
int Controller::ping(int* ids)
{
    // Broadcast ping
    sendPingPacket();

    // Read data from serial 50ms
    receiveData(50000);

    // Parse each packet
    Packet pack;
    int sts;
    int number = 0;
    while( (sts=getNextPacket(pack)) != RxBufferEmpty )
    {
        if(sts != 0) continue;
        // Check that it is a status packet
        if(pack.getInstruction() == InsStatus)
        {
            ids[number] = (int) pack.getId();
            number++;
        }
        // else drop packet (it is our TX)
    }
    return number;
}

/* ============================================================================
 *
 * */
int Controller::getNumber(int* values) const
{
    if(mNumberOfSelectedServo == 1) {
        sendReadPacket(mSelectedServoIds[0], CiModelNumber);
        return readValuesFromRxPackets(values);
    }
    else {

    }

}

/* ============================================================================
 *
 * */
int Controller::getVersion(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setId(byte id)
{
    if(mNumberOfSelectedServo >= 1) {
        sendWritePacket(mSelectedServoIds[0], CiId, id);
        receiveData(50); // drop packet
    }
}

/* ============================================================================
 *
 * */
int Controller::getBaud(BaudRate* brs) const
{
    int values[mNumberOfSelectedServo];
    
    sendReadPacket(mSelectedServoIds[0], CiBaudRate);
    delay(50);
    readValuesFromRxPackets(values);

    for(int i=0 ; i<mNumberOfSelectedServo ; i++)
        brs[i] = (BaudRate)values[i];
}

/* ============================================================================
 *
 * */
void Controller::setBaud(BaudRate br) const
{
    if(mNumberOfSelectedServo >= 1)
    {
        sendWritePacket(mSelectedServoIds[0], CiBaudRate, (int)br);
        // TODO DROP PACKETS
    }
}

/* ============================================================================
 *
 * */
int Controller::getReturnDelayTime(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setReturnDelayTime(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getCwAngleLimit(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setCwAngleLimit(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getCcwAngleLimit(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setCcwAngleLimit(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getControlMode(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setControlMode(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getLimitTemperature(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setLimitTemperature(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getDownLimitVoltage(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setDownLimitVoltage(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getUpLimitVoltage(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setUpLimitVoltage(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getMaxTorque(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setMaxTorque(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getReturnLevel(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setReturnLevel(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getAlarmShutdown(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setAlarmShutdown(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getTorqueEnable(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setTorqueEnable(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getLed(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setLed(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getDgain(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setDgain(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getIgain(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setIgain(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getPgain(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setPgain(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getGoalPosition(int* positions) const
{

}
/* ============================================================================
 *
 * */
void Controller::setGoalPosition(const int* positions, int number) const
{
    // if(mNumberOfSelectedServo == 1)
    //     sendWritePacket(mSelectedServoIds[0], CiGoalPosition, positions[0]);
}

/* ============================================================================
 *
 * */
int Controller::getGoalSpeed(int* speeds) const
{
// int Controller::getGpos(int* positions) const
// {
//     sendReadPacket(mSelectedServoIds[0], CiGoalPosition);
//     delay(50);
//     readValuesFromRxPackets(positions);
// }
}

/* ============================================================================
 *
 * */
void Controller::setGoalSpeed(const int* speeds, int number) const
{
    if(mNumberOfSelectedServo == 1)
        sendWritePacket(mSelectedServoIds[0], CiGoalSpeed, speeds[0]);
}

/* ============================================================================
 *
 * */
int Controller::getGoalTorque(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setGoalTorque(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getPresentPosition(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setPresentPosition(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getPresentSpeed(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setPresentSpeed(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getPresentLoad(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setPresentLoad(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getPresentVoltage(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setPresentVoltage(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getPresentTemperature(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setPresentTemperature(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getRegisteredInstruction(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setRegisteredInstruction(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getMoving(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setMoving(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getHardwareError(int* values) const
{

}

/* ============================================================================
 *
 * */
void Controller::setHardwareError(const int* values, int number) const
{

}

/* ============================================================================
 *
 * */
int Controller::getPunch(int* values) const
{

}
/* ============================================================================
 *
 * */
void Controller::setPunch(const int* values, int number) const
{

}
