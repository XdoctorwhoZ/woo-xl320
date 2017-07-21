#ifndef XXCMDMACHINE_HPP
#define XXCMDMACHINE_HPP
#pragma once

// Std
#include <stdio.h>
#include <stdlib.h>

// Arduino
#include <Stream.h>
#include <Arduino.h>

// Woo
#include "XxCmd.hpp"
#include "XL320Controller.hpp"

//! Machine to interpret command from serial
//!
class XxCmdMachine
{

public:

    //! Maximal number of servo xl-320 selectable at once
    static constexpr byte MaxCmdSize = 128;

private:

    //! Stream to read commands and write status
    //!
    Stream* mStream;

    //! Buffer to store input command
    //!
    char mCmdBuffer[MaxCmdSize];

    //! Pointer on the next case of the buffer that can be filled
    //!
    char* mCmdPtr;

    //! Controller for xl-320 servo chain
    //!
    xl320::Controller mController;

public:

    //! Constructor
    //!
    XxCmdMachine();

    //! Initialize the machine with a stream to read command and write status
    //!
    void setup(Stream& cmdStream, xl320::Controller::XlSerial& xlSerial);

    //! Check if there are some commands in the stream
    //!
    void loop();

    //! Command parsing
    //!
    void parse(const char* command);

private:

    // === Command buffer management ===

    //! Reset cmd ptr
    //!
    void rstCmdPtr() { mCmdPtr = mCmdBuffer; }

    //! Add c to the cmd buffer
    //!
    void addCmdChar(char c);

    // === Reply helper ===

    //! 
    //!
    void checkWarning(int left);

    //! To send message back to user
    //!
    void reply(const char* msg) { mStream->write(msg); }

    //! To reply an error
    //!
    int syntaxError(const char* what = "")
    {
        String msg = "ERROR:";
        msg += what;
        msg += "\r\n";
        reply(msg.c_str());
        return -1;
    }

    //! cmd = +CMD:
    //! param = info that must be send in string form
    //!
    void replyGetterWithParam(const char* cmd, const char* param);

    //! cmd = +CMD:
    //! a list that must be display with ',' between each vlaue
    //!
    void replyGetterWithIntList(const char* cmd, const int* list, int size);
    void replyGetterWithIntList(XxCmd::Value command, const int* list, int size);

    // === Command functions ===

    //! XX+SELECT?
    void cmdXbaudGetter();
    void cmdPingGetter();
    void cmdSelectGetter();

    void cmdModelNumberGetter();             
    void cmdVersionGetter();                 
    void cmdIdGetter();                      
    void cmdBaudGetter();                    
    void cmdReturnDelayTimeGetter();         
    void cmdCwAngleLimitGetter();            
    void cmdCcwAngleLimitGetter();           
    void cmdControlModeGetter();             
    void cmdLimitTemperatureGetter();        
    void cmdDownLimitVoltageGetter();        
    void cmdUpLimitVoltageGetter();          
    void cmdMaxTorqueGetter();               
    void cmdReturnLevelGetter();             
    void cmdAlarmShutdownGetter();           
    void cmdTorqueEnableGetter();            
    void cmdLedGetter();                     
    void cmdDgainGetter();                   
    void cmdIgainGetter();                   
    void cmdPgainGetter();                   
    void cmdGoalPositionGetter();            
    void cmdGoalSpeedGetter();               
    void cmdGoalTorqueGetter();              
    void cmdPresentPositionGetter();         
    void cmdPresentSpeedGetter();            
    void cmdPresentLoadGetter();             
    void cmdPresentVoltageGetter();          
    void cmdPresentTemperatureGetter();      
    void cmdRegisteredInstructionGetter();   
    void cmdMovingGetter();                  
    void cmdHardwareErrorGetter();           
    void cmdPunchGetter();                   

    //! XX+SELECT=
    void cmdXbaudSetter(const char* args);
    void cmdSelectSetter(const char* args);

    void cmdModelNumberSetter(const char* args);             
    void cmdVersionSetter(const char* args);                 
    void cmdIdSetter(const char* args);                      
    void cmdBaudSetter(const char* args);                    
    void cmdReturnDelayTimeSetter(const char* args);         
    void cmdCwAngleLimitSetter(const char* args);            
    void cmdCcwAngleLimitSetter(const char* args);           
    void cmdControlModeSetter(const char* args);             
    void cmdLimitTemperatureSetter(const char* args);        
    void cmdDownLimitVoltageSetter(const char* args);        
    void cmdUpLimitVoltageSetter(const char* args);          
    void cmdMaxTorqueSetter(const char* args);               
    void cmdReturnLevelSetter(const char* args);             
    void cmdAlarmShutdownSetter(const char* args);           
    void cmdTorqueEnableSetter(const char* args);            
    void cmdLedSetter(const char* args);                     
    void cmdDgainSetter(const char* args);                   
    void cmdIgainSetter(const char* args);                   
    void cmdPgainSetter(const char* args);                   
    void cmdGoalPositionSetter(const char* args);            
    void cmdGoalSpeedSetter(const char* args);               
    void cmdGoalTorqueSetter(const char* args);              
    void cmdPresentPositionSetter(const char* args);         
    void cmdPresentSpeedSetter(const char* args);            
    void cmdPresentLoadSetter(const char* args);             
    void cmdPresentVoltageSetter(const char* args);          
    void cmdPresentTemperatureSetter(const char* args);      
    void cmdRegisteredInstructionSetter(const char* args);   
    void cmdMovingSetter(const char* args);                  
    void cmdHardwareErrorSetter(const char* args);           
    void cmdPunchSetter(const char* args);                   

};

//! Class to parse command arguments
//!
class XxArgParser
{
    const char* mArgs;
    const char* mPtr;

public:

    //! Constructor with args string
    //!
    XxArgParser(const char* args) : mArgs(args) { mPtr = mArgs; }

    //! Fill arg with the arg string value
    //! return 1 if an arg has been found
    //! return 0 else
    //!
    byte getNextArg(char* arg)
    {
        /*
        Serial.print("===");
        Serial.println(*mPtr);
        Serial.println((int)*mPtr);
    */
        // Check if there is more
        if(*mPtr == '\r' || *mPtr == '\n' || *mPtr == '\0')
        {
            *arg = '\0';
            // Serial.println("return 0");            
            return 0;
        }

        // Skip ','
        if(*mPtr == ',') mPtr++;
     
        // Get next arg
        while(*mPtr != ',' && *mPtr != '\r' && *mPtr != '\n' && *mPtr != '\0')
        {
            *arg = *mPtr;
        //    Serial.println(*arg);
          //  Serial.println(*mPtr);
            arg++;
            mPtr++;
        }

        // End it
        *arg = '\0';

      //      Serial.println("return 1");
        return 1;
    }


    int toIntList(int* li)
    {
        int n = 0;
        char arg[8];
        while(getNextArg(arg))
        {
            li[n] = String(arg).toInt();
            n++;
        }
        return 0;
    }

};



#endif // XXCMDMACHINE_HPP

