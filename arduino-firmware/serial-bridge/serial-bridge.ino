#include <Stream.h>
#include <Arduino.h>

#define PC_SERIAL Serial
#define XL_SERIAL Serial3

//!
#define READ_BUFFER_SIZE 128
byte ReadBuffer[READ_BUFFER_SIZE];

//! Setup function
void setup()
{
    PC_SERIAL.begin(115200);
    XL_SERIAL.begin(115200);
}

//! Loop function
void loop()
{
    int readSize = 0;

    // PC -> XL
    if ( (readSize = PC_SERIAL.available()) > 0 )
    {
        // Read Received data
        if( readSize > READ_BUFFER_SIZE )
        {
            readSize = READ_BUFFER_SIZE;
        }
        PC_SERIAL.readBytes(ReadBuffer, readSize);

        // Send it directly to XL
        XL_SERIAL.write(ReadBuffer, readSize);
    }

    // XL -> PC
    if ( (readSize = XL_SERIAL.available()) > 0 )
    {
        // Read Received data
        if( readSize > READ_BUFFER_SIZE )
        {
            readSize = READ_BUFFER_SIZE;
        }
        XL_SERIAL.readBytes(ReadBuffer, readSize);

        // Send it directly to PC
        PC_SERIAL.write(ReadBuffer, readSize);
    }
 
}
