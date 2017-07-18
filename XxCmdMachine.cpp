#include "XxCmdMachine.hpp"

/* ============================================================================
 *
 * */
XxCmdMachine::XxCmdMachine()
    : 
     mCurrentId(1)
    , mCurrentBaudRate(Br1Mbps)
{
    mCmdPtr = mCmdBuffer;
}

/* ============================================================================
 *
 * */
void XxCmdMachine::setup(Stream& cmdStream, XL320::XlSerial& xlSerial)
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
                default: return syntaxError("unknown");
            }
        }
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
    }

    // Check status
    if(sts)
    {
        reply("ERROR\r\n");
    }
    else
    {
        reply("OK\r\n");
    }
    return sts;
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
    String msg = "+XBAUD:" + mController.getXlBaudRateString() + "\r\n";
    reply(msg.c_str());
    return 0;
}

/* ============================================================================
 *
 * */
int XxCmdMachine::cmdSelectGetter()
{
    String msg = "+SELECT:" + mController.getSelectedServoString() + "\r\n";
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

    XL320::BaudRate br;
    if      (baud == "9600") {
        br = XL320::Br9600;
    }
    else if (baud == "57600") {
        br = XL320::Br57600;
    }
    else if (baud == "115200") {
        br = XL320::Br115200;
    }
    else if (baud == "1000000") {
        br = XL320::Br1Mbps;
    }
    else {
        return 50;
    }

    mController.setXlBaudRate(br);

    String msg = "+XBAUD:" + mController.getXlBaudRateString() + "\r\n";
    reply(msg.c_str());

    return 0;
}

/* ============================================================================
 *
 * */
int XxCmdMachine::cmdSelectSetter(const char* args)
{
    byte number = 0;
    byte ids[XL320::MaxServoSelectable];

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

    String msg = "+SELECT=" + mController.getSelectedServoString() + "\r\n";
    reply(msg.c_str());

    return 0;
}

/* ============================================================================
 *
 * */
int XxCmdMachine::cmdGposSetter(const char* args)
{
    
}
