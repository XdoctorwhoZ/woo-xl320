
// Woo
#include "XxCmdMachine.hpp"

//! Machine to parse XX commands
//!
XxCmdMachine CmdMachine;

//! Setup function
//!
void setup()
{
    Serial.begin(115200); // cmd stream
    Serial3.begin(115200); // xl serial
    CmdMachine.setup(Serial, Serial3);
}

//! Loop function
//!
void loop()
{
    CmdMachine.loop();
}
