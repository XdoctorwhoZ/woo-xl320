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
        PC_SERIAL.print("+ Write on servo ");
        PC_SERIAL.println(servoIds[i]);

        // Select the servo that we are going to read 
        byte servo = servoIds[i];
        controller.selectServo(&servo, 1);

        // Create a table for the read values
        // There will be one value per servo selected
        int values[numberOfServo];



        // controller.pull(xl320::RegIndex::ReturnDelayTime, values);
        // PC_SERIAL.print("    - ReturnDelayTime: ");
        // PC_SERIAL.println(values[0]);
                              
        // controller.pull(xl320::RegIndex::CwAngleLimit, values);
        // PC_SERIAL.print("    - CwAngleLimit: ");
        // PC_SERIAL.println(values[0]);

        // controller.pull(xl320::RegIndex::CcwAngleLimit, values);
        // PC_SERIAL.print("    - CcwAngleLimit: ");
        // PC_SERIAL.println(values[0]);
           
        // controller.pull(xl320::RegIndex::ControlMode, values);
        // PC_SERIAL.print("    - ControlMode: ");
        // PC_SERIAL.println(values[0]);



                   
        // controller.pull(xl320::RegIndex::TorqueEnable, values);
        // PC_SERIAL.print("    - TorqueEnable: ");
        // PC_SERIAL.println(values[0]); 

        // controller.pull(xl320::RegIndex::Led, values);
        // PC_SERIAL.print("    - Led: ");
        // PC_SERIAL.println(values[0]);                   


        // controller.pull(xl320::RegIndex::GoalPosition, values);
        // PC_SERIAL.print("    - GoalPosition: ");
        // PC_SERIAL.println(values[0]);

        // controller.pull(xl320::RegIndex::GoalSpeed, values);
        // PC_SERIAL.print("    - GoalSpeed: ");
        // PC_SERIAL.println(values[0]);




    }

    // Wait 10sec
    delay(10000);
}
