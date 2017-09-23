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

    static uint16_t speed = 0;

    //
    for(uint8_t i=0 ; i<numberOfServo ; i++)
    {
        PC_SERIAL.print("+ Control servo ");
        PC_SERIAL.println(servoIds[i]);

        // Select the servo that we are going to read 
        uint8_t servo = servoIds[i];
        controller.selectServo(&servo, 1);

        // 
        static uint16_t write_value[1];

        // 
        PC_SERIAL.print("+ Speed ");
        PC_SERIAL.println((int)speed);
        write_value[0] = speed;
        controller.push(xl320::RegIndex::GoalSpeed, write_value);
        speed = (speed+300)%1023;

        //
        delay(1000);

        //
        PC_SERIAL.println("+ Set pos 0");
        write_value[0] = 0;
        controller.push(xl320::RegIndex::GoalPosition, write_value);

        //
        delay(3000);

        //
        PC_SERIAL.println("+ Set pos 512");
        write_value[0] = 512;
        controller.push(xl320::RegIndex::GoalPosition, write_value);

        //
        delay(3000);

        //
        PC_SERIAL.println("+ Set pos 1023");
        write_value[0] = 1023;
        controller.push(xl320::RegIndex::GoalPosition, write_value);

    }

    // Wait
    PC_SERIAL.println("===================================================");
    delay(3 * 1000);
}
