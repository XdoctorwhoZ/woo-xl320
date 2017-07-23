// Woo
#include "XxCmdMachine.hpp"

// ---
using namespace xl320;

/* ============================================================================
 *
 * */
XxCmdMachine::XxCmdMachine()
{
    mCmdPtr = mCmdBuffer;
}

/* ============================================================================
 *
 * */
void XxCmdMachine::setup(Stream& cmdStream, xl320::Controller::XlSerial& xlSerial)
{
    mStream = &cmdStream;
    mController.setup(xlSerial);
}

/* ============================================================================
 *
 * */
void XxCmdMachine::loop()
{
    // Get input char and parse if needed
    while (mStream->available())
    {
        char c = mStream->read();
        addCmdChar(c);
        if (c=='\n')
        {
            addCmdChar('\0');
            parse(mCmdBuffer);
            rstCmdPtr();
        }
    }
    delay(100);
}

/* ============================================================================
 *
 * */
void XxCmdMachine::parse(const char* command)
{
    const char* ptr = command;

    // Check XX
    for (int i=0 ; i<2 ; i++)
    {
        if (*ptr != 'X') { syntaxError("start XX"); return; }
        ptr++;
    }

    // Parse command
    if (*ptr == '+')
    {
        ptr++;

        // Get string limits
        const char* cmdbeg = ptr;
        while( *ptr != '\r' && *ptr != '?' && *ptr != '=' && *ptr != '\0' ) ptr++;

        // Interpret string
        XxCmd::Value cmd = XxCmd::GetValueFromName(cmdbeg, ptr);
        if (cmd == XxCmd::Total) {
            syntaxError(); return;
        }

        // Check if this is a getter of setters function
        if (*ptr != '?' && *ptr != '=') {
            syntaxError("need ? or ="); return;
        }

        // Getter command
        if (*ptr == '?')
        {
            // execute command XX+CMD?
            switch(cmd)
            {
                case XxCmd::XBaud                   : cmdXbaudGetter();                 break;
                case XxCmd::Ping                    : cmdPingGetter();                  break;
                case XxCmd::Select                  : cmdSelectGetter();                break;

                case XxCmd::ModelNumber             : cmdModelNumberGetter();           break;
                case XxCmd::Version                 : cmdVersionGetter();               break;
                case XxCmd::Id                      : cmdIdGetter();                    break;
                case XxCmd::Baud                    : cmdBaudGetter();                  break;
                case XxCmd::ReturnDelayTime         : cmdReturnDelayTimeGetter();       break;
                case XxCmd::CwAngleLimit            : cmdCwAngleLimitGetter();          break;
                case XxCmd::CcwAngleLimit           : cmdCcwAngleLimitGetter();         break;
                case XxCmd::ControlMode             : cmdControlModeGetter();           break;
                case XxCmd::LimitTemperature        : cmdLimitTemperatureGetter();      break;
                case XxCmd::DownLimitVoltage        : cmdDownLimitVoltageGetter();      break;
                case XxCmd::UpLimitVoltage          : cmdUpLimitVoltageGetter();        break;
                case XxCmd::MaxTorque               : cmdMaxTorqueGetter();             break;
                case XxCmd::ReturnLevel             : cmdReturnLevelGetter();           break;
                case XxCmd::AlarmShutdown           : cmdAlarmShutdownGetter();         break;
                case XxCmd::TorqueEnable            : cmdTorqueEnableGetter();          break;
                case XxCmd::Led                     : cmdLedGetter();                   break;
                case XxCmd::Dgain                   : cmdDgainGetter();                 break;
                case XxCmd::Igain                   : cmdIgainGetter();                 break;
                case XxCmd::Pgain                   : cmdPgainGetter();                 break;
                case XxCmd::GoalPosition            : cmdGoalPositionGetter();          break;
                case XxCmd::GoalSpeed               : cmdGoalSpeedGetter();             break;
                case XxCmd::GoalTorque              : cmdGoalTorqueGetter();            break;
                case XxCmd::PresentPosition         : cmdPresentPositionGetter();       break;
                case XxCmd::PresentSpeed            : cmdPresentSpeedGetter();          break;
                case XxCmd::PresentLoad             : cmdPresentLoadGetter();           break;
                case XxCmd::PresentVoltage          : cmdPresentVoltageGetter();        break;
                case XxCmd::PresentTemperature      : cmdPresentTemperatureGetter();    break;
                case XxCmd::RegisteredInstruction   : cmdRegisteredInstructionGetter(); break;
                case XxCmd::Moving                  : cmdMovingGetter();                break;
                case XxCmd::HardwareError           : cmdHardwareErrorGetter();         break;
                case XxCmd::Punch                   : cmdPunchGetter();                 break;

                default                             : syntaxError("unknown");
            }
        }
        // Setter command XX+CMD=...
        else
        {
            // skip '='
            ptr++;

            // execute command
            switch(cmd)
            {
                case XxCmd::XBaud                   : cmdXbaudSetter(ptr) ;                     break;
                case XxCmd::Select                  : cmdSelectSetter(ptr);                     break;

                case XxCmd::Id                      : cmdIdSetter(ptr);                         break;
                case XxCmd::Baud                    : cmdBaudSetter(ptr);                       break;
                case XxCmd::ReturnDelayTime         : cmdReturnDelayTimeSetter(ptr);            break;
                case XxCmd::CwAngleLimit            : cmdCwAngleLimitSetter(ptr);               break;
                case XxCmd::CcwAngleLimit           : cmdCcwAngleLimitSetter(ptr);              break;
                case XxCmd::ControlMode             : cmdControlModeSetter(ptr);                break;
                case XxCmd::LimitTemperature        : cmdLimitTemperatureSetter(ptr);           break;
                case XxCmd::DownLimitVoltage        : cmdDownLimitVoltageSetter(ptr);           break;
                case XxCmd::UpLimitVoltage          : cmdUpLimitVoltageSetter(ptr);             break;
                case XxCmd::MaxTorque               : cmdMaxTorqueSetter(ptr);                  break;
                case XxCmd::ReturnLevel             : cmdReturnLevelSetter(ptr);                break;
                case XxCmd::AlarmShutdown           : cmdAlarmShutdownSetter(ptr);              break;
                case XxCmd::TorqueEnable            : cmdTorqueEnableSetter(ptr);               break;
                case XxCmd::Led                     : cmdLedSetter(ptr);                        break;
                case XxCmd::Dgain                   : cmdDgainSetter(ptr);                      break;
                case XxCmd::Igain                   : cmdIgainSetter(ptr);                      break;
                case XxCmd::Pgain                   : cmdPgainSetter(ptr);                      break;
                case XxCmd::GoalPosition            : cmdGoalPositionSetter(ptr);               break;
                case XxCmd::GoalSpeed               : cmdGoalSpeedSetter(ptr);                  break;
                case XxCmd::GoalTorque              : cmdGoalTorqueSetter(ptr);                 break;
                case XxCmd::PresentPosition         : cmdPresentPositionSetter(ptr);            break;
                case XxCmd::PresentSpeed            : cmdPresentSpeedSetter(ptr);               break;
                case XxCmd::PresentLoad             : cmdPresentLoadSetter(ptr);                break;
                case XxCmd::PresentVoltage          : cmdPresentVoltageSetter(ptr);             break;
                case XxCmd::PresentTemperature      : cmdPresentTemperatureSetter(ptr);         break;
                case XxCmd::RegisteredInstruction   : cmdRegisteredInstructionSetter(ptr);      break;
                case XxCmd::Moving                  : cmdMovingSetter(ptr);                     break;
                case XxCmd::HardwareError           : cmdHardwareErrorSetter(ptr);              break;
                case XxCmd::Punch                   : cmdPunchSetter(ptr);                      break;
                default                             : syntaxError("unknown");                   break;
            }
        }
    }
    else // case XX\r\n
    {
        if (*ptr != '\r') { syntaxError("end \\r\\n"); return; }
        ptr++;
        if (*ptr != '\n') { syntaxError("end \\r\\n"); return; }
        ptr++;
        reply("OK\r\n");
    }
}

/* ============================================================================
 *
 * */
void XxCmdMachine::addCmdChar(char c)
{
    if( (mCmdPtr-mCmdBuffer) < MaxCmdSize )
    {
        *mCmdPtr = c; mCmdPtr++;
    }
}

/* ============================================================================
 *
 * */
void XxCmdMachine::checkWarning(int left)
{
    if(left) {
        String msg("#Partial reply: ");
        msg += left;
        msg += " servo does not respond\r\n";
        mStream->write(msg.c_str());
        mStream->flush();
    }

    if(mController.getHwError()) {
        String msg("#HwError:n=");
        msg += (int) mController.getHwError();
        msg += ", last=" + Controller::ErrorToString(mController.getLastError());
        mStream->write(msg.c_str());
        mStream->flush();
    }
}

/* ============================================================================
 *
 * */
void XxCmdMachine::replyGetterWithParam(const char* cmd, const char* param)
{
    String msg(cmd);
    msg += param;
    msg += "\r\nOK\r\n";
    reply(msg.c_str());
}

/* ============================================================================
 *
 * */
void XxCmdMachine::replyGetterWithIntList(const char* cmd, const int* list, int size)
{
    String msg(cmd);
    for(int i=0 ; i<size ; i++)
    {
        if(i!=0) msg += ',';
        msg += (int) list[i];
    }
    msg += "\r\nOK\r\n";
    reply(msg.c_str());
}

/* ============================================================================
 *
 * */
void XxCmdMachine::replyGetterWithIntList(XxCmd::Value command, const int* list, int size)
{
    String cmd("+");
    cmd += XxCmd::Names[command];
    cmd += ':';
    replyGetterWithIntList(cmd.c_str(), list, size);
}

/* ============================================================================
 *
 * */
void XxCmdMachine::stdListGetter(GetFctPtr getfct, XxCmd::Value val)
{
    const int num = mController.getNumberOfSelectedServo();
    int values[num];
    checkWarning((mController.*getfct)(values));
    replyGetterWithIntList(val, values, num);
}

/* ============================================================================
 *
 * */
void XxCmdMachine::stdListSetter(SetFctPtr setfct, const char* args)
{
    int number = 0;
    int values[Controller::MaxServoSelectable];

    XxArgParser ap(args);
    number = ap.toIntList(values);
    (mController.*setfct)(values, number);

    replyOk();
}

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdXbaudGetter()
{
    replyGetterWithParam("+XBAUD:", mController.getXlBaudRateString().c_str());
}

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdPingGetter()
{
    int ids[Controller::MaxServoSelectable];
    int num = mController.ping(ids);
    replyGetterWithIntList("+PING:", ids, num);
}

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdSelectGetter()
{
    String msg = "+SELECT:" + mController.getSelectedServoString() + "\r\nOK\r\n";
    reply(msg.c_str());
}

/* ============================================================================
 *
 * *
void XxCmdMachine::cmdGposGetter()
{
    const int num = mController.getNumberOfSelectedServo();

    // Send request to servos
    int values[num];
    mController.getGpos(values);

    // prepare answer
    String msg = "+GPOS:";
    for(int i=0 ; i<num ; i++)
    {
        if(i!=0) msg += ',';
        msg += (int) values[i];
    }
    msg += "\r\nOK\r\n";
    reply(msg.c_str());
    return 0;
}

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdModelNumberGetter()
{
    const int num = mController.getNumberOfSelectedServo();
    int values[num];
    checkWarning(mController.getNumber(values));
    replyGetterWithIntList(XxCmd::ModelNumber, values, num);
}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdVersionGetter()
{
    const int num = mController.getNumberOfSelectedServo();
    int values[num];
    checkWarning(mController.getVersion(values));
    replyGetterWithIntList(XxCmd::ModelNumber, values, num);
}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdIdGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdBaudGetter()
{
    const int num = mController.getNumberOfSelectedServo();
    BaudRate values[num];
    mController.getBaud(values);

    // prepare answer
    String msg;
    for(int i=0 ; i<num ; i++)
    {
        if(i!=0) msg += ',';
        switch(values[i])
        {
            case Br9600   : msg+= "9600";    break;
            case Br57600  : msg+= "57600";   break;
            case Br115200 : msg+= "115200";  break;
            case Br1Mbps  : msg+= "1000000"; break;
            default       : msg+= "???";     break;
        }
    }
    replyGetterWithParam("+BAUD:", msg.c_str());
}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdReturnDelayTimeGetter()
{
    const int num = mController.getNumberOfSelectedServo();
    int values[num];
    checkWarning(mController.getReturnDelayTime(values));
    replyGetterWithIntList(XxCmd::ModelNumber, values, num);
}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdCwAngleLimitGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdCcwAngleLimitGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdControlModeGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdLimitTemperatureGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdDownLimitVoltageGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdUpLimitVoltageGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdMaxTorqueGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdReturnLevelGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdAlarmShutdownGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdTorqueEnableGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdLedGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdDgainGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdIgainGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdPgainGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdGoalPositionGetter()
{
    const int num = mController.getNumberOfSelectedServo();
    int values[num];
    checkWarning(mController.getGoalPosition(values));
    replyGetterWithIntList(XxCmd::GoalPosition, values, num);
}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdGoalSpeedGetter()
{
    const int num = mController.getNumberOfSelectedServo();
    int values[num];
    checkWarning(mController.getGoalSpeed(values));
    replyGetterWithIntList(XxCmd::GoalSpeed, values, num);
}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdGoalTorqueGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdPresentPositionGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdPresentSpeedGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdPresentLoadGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdPresentVoltageGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdPresentTemperatureGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdRegisteredInstructionGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdMovingGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdHardwareErrorGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdPunchGetter()
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdXbaudSetter(const char* args)
{
    char arg[16];
    XxArgParser ap(args);

    ap.getNextArg(arg);
    String baud(arg);

    BaudRate br;
    if      (baud == "9600") {
        br = Br9600;
    }
    else if (baud == "57600") {
        br = Br57600;
    }
    else if (baud == "115200") {
        br = Br115200;
    }
    else if (baud == "1000000") {
        br = Br1Mbps;
    }
    else {
        // manage error here
        // return 50;
    }

    mController.setXlBaudRate(br);

    String msg = "OK" + mController.getXlBaudRateString() + "\r\n";
    reply(msg.c_str());
}

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdSelectSetter(const char* args)
{
    byte number = 0;
    byte ids[Controller::MaxServoSelectable];

    char arg[8];
    XxArgParser ap(args);

    while(ap.getNextArg(arg))
    {
        #ifdef XXXDEBUG
            Serial.print("arg: ");
            Serial.println(arg);
        #endif // XXXDEBUG

        ids[number] = String(arg).toInt();

        #ifdef XXXDEBUG
            Serial.print("ids: ");
            Serial.println(ids[number]);
        #endif // XXXDEBUG

        number++;
    }

    mController.selectServo(ids, number);

    String msg = "OK" + mController.getSelectedServoString() + "\r\n";
    reply(msg.c_str());
}

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdIdSetter(const char* args)
{
    char arg[16];
    XxArgParser ap(args);

    ap.getNextArg(arg);
    String idstr(arg);
    byte id = idstr.toInt();

    mController.setId(id);
}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdBaudSetter(const char* args)
{
    char arg[16];
    XxArgParser ap(args);

    ap.getNextArg(arg);
    String baud(arg);

    BaudRate br;
    if      (baud == "9600") {
        br = Br9600;
    }
    else if (baud == "57600") {
        br = Br57600;
    }
    else if (baud == "115200") {
        br = Br115200;
    }
    else if (baud == "1000000") {
        br = Br1Mbps;
    }
    else {
        // manage error here
        // return 50;
    }

    mController.setBaud(br);

}

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdReturnDelayTimeSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdCwAngleLimitSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdCcwAngleLimitSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdControlModeSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdLimitTemperatureSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdDownLimitVoltageSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdUpLimitVoltageSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdMaxTorqueSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdReturnLevelSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdAlarmShutdownSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdTorqueEnableSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdLedSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdDgainSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdIgainSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdPgainSetter(const char* args)
{

}   


    // int getGoalTorque(int* values) { return getStdVals(values, CiGoalTorque); }
    // int getGoalSpeed(int* values) { return getStdVals(values, CiGoalSpeed); }
    // int getPresentPosition(int* values) { return getStdVals(values, CiPresentPosition); }
    // int getPresentSpeed(int* values) { return getStdVals(values, CiPresentSpeed); }
    // int getPresentLoad(int* values) { return getStdVals(values, CiPresentLoad); }    
    // int getPresentVoltage(int* values) { return getStdVals(values, CiPresentVoltage); }
    // int getPresentTemperature(int* values) { return getStdVals(values, CiPresentTemperature); }
    



/* ============================================================================
 *
 * */
void XxCmdMachine::cmdGoalPositionSetter(const char* args)
{
    int number = 0;
    int values[Controller::MaxServoSelectable];

    XxArgParser ap(args);
    number = ap.toIntList(values);
    mController.setGoalPosition(values, number);
    
    replyOk();
}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdRegisteredInstructionSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdMovingSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdHardwareErrorSetter(const char* args)
{

}   

/* ============================================================================
 *
 * */
void XxCmdMachine::cmdPunchSetter(const char* args)
{

} 
