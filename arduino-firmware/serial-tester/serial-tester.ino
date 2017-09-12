#include <Stream.h>
#include <Arduino.h>

#define PC_SERIAL Serial

//!
#define READ_BUFFER_SIZE 128
byte ReadBuffer[READ_BUFFER_SIZE];

long int counter;

//! Setup function
void setup()
{
    counter = 0;
    PC_SERIAL.begin(115200);
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
        PC_SERIAL.write(ReadBuffer, readSize);
    }

    PC_SERIAL.print(counter);
    counter++;

    // test with 1s delay
    delay(500);
}
