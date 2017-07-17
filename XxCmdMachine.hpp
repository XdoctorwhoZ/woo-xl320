#ifndef XXCMDMACHINE_HPP
#define XXCMDMACHINE_HPP
#pragma once


#include <stdlib.h>
#include <stdio.h>

#include <Stream.h>
#include <Arduino.h>

#include "XxCmd.hpp"


//!
//!
class XxCmdMachine
{
    enum BaudRate { Br9600, Br57600, Br115200, Br1Mbps };


    //! Stream to read commands and write status
    //!
    Stream* mStream;

    //! Buffer to store input command
    //!
    char mCmdBuffer[128];

    //! Pointer on the next case of the buffer that can be filled
    //!
    char* mCmdPtr;

    //! Id of the current joint controlled
    //!
    int mCurrentId;

    //! Baudrate used by the serial driver to communicate with xl device
    //!
    BaudRate mCurrentBaudRate;



public:

    //! Constructor
    //!
    XxCmdMachine();

    //! Initialize the machine with a stream to read command and write status
    //!
    void setup(Stream* stream);

    //! Check if there are some commands in the stream
    //!
    void loop();

    //! Command parsing
    //!
    int parse(const char* command);

private:


    int cmdSelectGetter();


    int reply(const char* msg) { mStream->write(msg);  }

    int syntaxError(const char* what = "")
    { 
        String msg = "ERROR:";
        msg += what;
        msg += "\r\n";
        reply(msg.c_str());
        return -1;
      }



};


#endif // XXCMDMACHINE_HPP
