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
void XxCmdMachine::setup(Stream* stream)
{
    mStream = stream;
}

/* ============================================================================
 *
 * */
void XxCmdMachine::loop()
{
    while (mStream->available())
    {
        char c = mStream->read();
        *mCmdPtr = c;
        mCmdPtr++;
        if (c=='\n')
        {
            *mCmdPtr = '\0';
            parse(mCmdBuffer);
            mCmdPtr = mCmdBuffer;
        }
    }
}

/* ============================================================================
 *
 * */
int XxCmdMachine::parse(const char* command)
{
    const char* ptr = command;

    // Check XX
    for (int i=0 ; i<2 ; i++)
    {
        if (*ptr != 'X') {
            return syntaxError();
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
            switch(cmd)
            {
                case XxCmd::Select: cmdSelectGetter(); break; 

                default: return syntaxError();
            }
        }
        // Setter command
        else
        {

        }

    }
    else
    {
        if (*ptr != '\r') {
            return syntaxError();
        }
        ptr++;
        if (*ptr != '\n') {
            return syntaxError();
        }
        ptr++;
    }


    // check XX
    // check +
    // check CMD


    reply("OK\r\n");
    return 0;
}

/* ============================================================================
 *
 * */
int XxCmdMachine::cmdSelectGetter()
{
    String msg = "+SELECT:";
    msg += String(mCurrentId);
    msg += "\r\n";
    reply(msg.c_str());
}

/* ============================================================================
 *
 * */
// int XxCmdMachine::cmdSelectSetter()
// {
    
// }


