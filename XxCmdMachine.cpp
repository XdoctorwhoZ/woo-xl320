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

    // Set a delay to account for the receive delay period
    delay(100);
}

/* ============================================================================
 *
 * */
int XxCmdMachine::parse(const char* command)
{
    int sts = 0;
    const char* ptr = command;

    // Check XX
    for (int i=0 ; i<2 ; i++)
    {
        if (*ptr != 'X') {
            return syntaxError("start XX");
        }
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
            return syntaxError();
        }

        // Check if this is a getter of setters function
        if (*ptr != '?' && *ptr != '=') {
            return syntaxError("need ? or =");
        }

        // Getter command
        if (*ptr == '?')
        {
            // execute command
            switch(cmd)
            {
                case XxCmd::XBaud:  sts = cmdXbaudGetter() ; break;
                case XxCmd::Ping:   sts = cmdPingGetter(); break;
                case XxCmd::Select: sts = cmdSelectGetter(); break;
                // case XxCmd::Gpos  : 
                default: return syntaxError("unknown");
            }
        }
        // Setter command
        else
        {
            // skip =
            ptr++;

            // execute command
            switch(cmd)
            {
                case XxCmd::XBaud:  sts = cmdXbaudSetter(ptr) ; break;
                case XxCmd::Select: sts = cmdSelectSetter(ptr); break; 
                case XxCmd::Gpos:   sts = cmdGposSetter(ptr);   break; 
                default: return syntaxError("unknown");
            }
        }
        return 0;
    }
    else
    {
        if (*ptr != '\r') {
            return syntaxError("end \\r\\n");
        }
        ptr++;
        if (*ptr != '\n') {
            return syntaxError("end \\r\\n");
        }
        ptr++;
        reply("OK\r\n");
        return 0;
    }

    reply("ERROR\r\n");
    return 1;
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
int XxCmdMachine::cmdXbaudGetter()
{
    String msg = "+XBAUD:" + mController.getXlBaudRateString() + "\r\nOK\r\n";
    reply(msg.c_str());
    return 0;
}

/* ============================================================================
 *
 * */
int XxCmdMachine::cmdPingGetter()
{
    byte ids[Controller::MaxServoSelectable];

    int num = mController.ping(ids);

    Serial.println(num, DEC);

    String msg = "+PING:";
    for(int i=0 ; i<num ; i++)
    {
        if(i!=0) msg += ',';
        msg += (int) ids[i];
    }
    msg += "\r\nOK\r\n";
    reply(msg.c_str());
    return 0;
}

/* ============================================================================
 *
 * */
int XxCmdMachine::cmdSelectGetter()
{
    String msg = "+SELECT:" + mController.getSelectedServoString() + "\r\nOK\r\n";
    reply(msg.c_str());
    return 0;
}

/* ============================================================================
 *
 * */
int XxCmdMachine::cmdGposGetter()
{

    return 0;
}

/* ============================================================================
 *
 * */
int XxCmdMachine::cmdXbaudSetter(const char* args)
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
        return 50;
    }

    mController.setXlBaudRate(br);

    String msg = "OK" + mController.getXlBaudRateString() + "\r\n";
    reply(msg.c_str());
    return 0;
}

/* ============================================================================
 *
 * */
int XxCmdMachine::cmdSelectSetter(const char* args)
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
    return 0;
}

/* ============================================================================
 *
 * */
int XxCmdMachine::cmdGposSetter(const char* args)
{
    int number = 0;
    int values[Controller::MaxServoSelectable];

    char arg[8];
    XxArgParser ap(args);

    while(ap.getNextArg(arg))
    {
        values[number] = String(arg).toInt();
        number++;
    }

    mController.setGpos(values, number);

    String msg = "OK\r\n";
    reply(msg.c_str());
    return 0;
}
