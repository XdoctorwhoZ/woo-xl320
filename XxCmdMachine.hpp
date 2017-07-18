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
#include "XL320.hpp"
#include "XxCmd.hpp"

//!
//!
class XxCmdMachine
{

public:

    //! Maximal number of servo xl-320 selectable at once
    static constexpr byte MaxCmdSize = 128;


    enum BaudRate { Br9600, Br57600, Br115200, Br1Mbps };

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
    XL320 mController;


public:

    //! Constructor
    //!
    XxCmdMachine();

    //! Initialize the machine with a stream to read command and write status
    //!
    void setup(Stream& cmdStream, XL320::XlSerial& xlSerial);

    //! Check if there are some commands in the stream
    //!
    void loop();

    //! Command parsing
    //!
    int parse(const char* command);

private:


    // === Command buffer management ===

    //! Reset cmd ptr
    //!
    void rstCmdPtr() { mCmdPtr = mCmdBuffer; }

    //! Add c to the cmd buffer
    //!
    void addCmdChar(char c);

    // === Command functions ===

    //! XX+SELECT?
    int cmdXbaudGetter();
    int cmdSelectGetter();
    int cmdGposGetter();

    //! XX+SELECT=
    int cmdXbaudSetter(const char* args);
    int cmdSelectSetter(const char* args);
    int cmdGposSetter(const char* args);




    void reply(const char* msg) { mStream->write(msg); }

    int syntaxError(const char* what = "")
    { 
        String msg = "ERROR:";
        msg += what;
        msg += "\r\n";
        reply(msg.c_str());
        return -1;
      }





};

//!
//!
class XxArgParser
{
    const char* mArgs;
    const char* mPtr;

public:

    XxArgParser(const char* args) : mArgs(args) { mPtr = mArgs; }

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

};



#endif // XXCMDMACHINE_HPP

