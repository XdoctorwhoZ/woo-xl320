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
    PC_SERIAL.println("###");
    PC_SERIAL.println("### START ###");

    // Get available servos
    static uint8_t servoIds[xl320::Controller::SelectSizeMax];
    static uint8_t numberOfServo; 
    numberOfServo = controller.ping(servoIds);
    PC_SERIAL.print("    - Found ");
    PC_SERIAL.print((int)numberOfServo);
    PC_SERIAL.println(" servos");

    // Read registers for each servos
    for(uint8_t i=0 ; i<numberOfServo ; i++)
    {
        PC_SERIAL.print("+ Read from servo ");
        PC_SERIAL.println(servoIds[i]);

        // Select the servo that we are going to read 
        uint8_t servo = servoIds[i];
        controller.selectServo(&servo, 1);

        // Create a table for the read values
        // There will be one value per servo selected
        static uint16_t read_value[8];

        controller.pull(xl320::RegIndex::ModelNumber, read_value);
        PC_SERIAL.print("#### ModelNumber: ");
        PC_SERIAL.println((int)read_value[0]);
        
        controller.pull(xl320::RegIndex::Version, read_value);
        PC_SERIAL.print("#### Version: ");
        PC_SERIAL.println((int)read_value[0]);

        controller.pull(xl320::RegIndex::Id, read_value);
        PC_SERIAL.print("#### Id: ");
        PC_SERIAL.println((int)read_value[0]);

        controller.pull(xl320::RegIndex::BaudRate, read_value);
        PC_SERIAL.print("#### BaudRate: ");
        PC_SERIAL.println((int)read_value[0]);

        controller.pull(xl320::RegIndex::PresentVoltage, read_value);
        PC_SERIAL.print("#### PresentVoltage: ");
        PC_SERIAL.println((int)read_value[0]);
    }

    // Wait
    PC_SERIAL.println("===================================================");
    delay(3 * 1000);
}
