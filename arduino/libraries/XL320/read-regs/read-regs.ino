#include <Stream.h>
#include <Arduino.h>

#include <XL320.h>

#define PC_SERIAL Serial
#define PC_BAUDRATE 115200
#define XL_SERIAL Serial3
#define XL_BAUDRATE 115200

//! Main object to control servos
xl320::Controller controller;

//! Setup function
void setup()
{
    PC_SERIAL.begin(PC_BAUDRATE);

    controller.setup(XL_SERIAL);
    controller.setXlBaudRate(XL_BAUDRATE);
}

//! Loop function
void loop()
{
    PC_SERIAL.println("+ Ping");

    // Get available servos
    byte servoIds[xl320::Constant::SelectSizeMax];
    byte numberOfServo = controller.ping(servoIds);
    PC_SERIAL.print("    - Found ");
    PC_SERIAL.print((int)numberOfServo);
    PC_SERIAL.println(" servos");

    // Read registers for each servos
    for(int i=0 ; i<numberOfServo ; i++)
    {
        PC_SERIAL.print("+ Read from servo ");
        PC_SERIAL.println(servoIds[i]);

        controller.selectServo(servoIds+i, 1);

        int values[numberOfServo];

        // Read id
        controller.pull(xl320::RegIndex::Id, values);
        PC_SERIAL.print("    - ID: ");
        PC_SERIAL.println(values[0]);

        // Read present voltage
        controller.pull(xl320::RegIndex::PresentVoltage, values);
        PC_SERIAL.print("    - Voltage: ");
        PC_SERIAL.println(values[0]);
    }



    // Wait 10sec
    delay(10000);
}
