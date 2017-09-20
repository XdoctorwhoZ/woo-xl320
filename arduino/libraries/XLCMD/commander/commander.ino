#include <XLCMD.h>

#define PC_SERIAL Serial
#define PC_BAUDRATE 115200
#define XL_SERIAL Serial3
#define XL_BAUDRATE 115200

xlcmd::Machine mMachine;

//! Setup function
void setup()
{
    PC_SERIAL.begin(PC_BAUDRATE);
    mMachine.setup(PC_SERIAL, XL_SERIAL);
}

//! Loop function
void loop()
{
    PC_SERIAL.println("=== XL CMD MACHINE ===");
    mMachine.loop();
}
