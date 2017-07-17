
// ========================================
// Dynamixel XL-320 Arduino library example
// ========================================

// Read more:
// https://github.com/hackerspace-adelaide/XL320

#include "XL320.h"
#include "XxCmdMachine.hpp"

// Name your robot!
XL320 robot;

// Set some variables for incrementing position & LED colour
char rgb[] = "rgbypcwo";
int servoPosition = 0;
int ledColour = 0;

// Set the default servoID to talk to
int servoID = 1;


//! Machine to parse XX commands
//!
XxCmdMachine CmdMachine;

//! Setup function
//!
void setup()
{

  // Talking standard serial, so connect servo data line to Digital TX 1
  // Comment out this line to talk software serial
  Serial.begin(115200);
  CmdMachine.setup(&Serial);

  // Setup Software Serial
  Serial3.begin(115200);

  // Initialise your robot
  robot.begin(Serial3); // Hand in the serial object you're using
  
  // I like fast moving servos, so set the joint speed to max!
  robot.setJointSpeed(servoID, 1023);

}

//! Loop function
//!
void loop()
{

    CmdMachine.loop();

  // Servo test.. select a random servoID and colour
//  robot.moveJoint(1, 512); // random(0, 1023)

  
  // Set a delay to account for the receive delay period
  delay(100);
}
