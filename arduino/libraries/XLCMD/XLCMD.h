#ifndef XLCMD_HPP
#define XLCMD_HPP
#pragma once

// 
#include <XL320.h>

// ---
namespace xlcmd {

//! Available XL commands index
enum CmdIndex
{
    XBaud                   =  0    ,
    Ping                    =  1    ,
    Select                  =  2    ,

    ModelNumber             =  3    ,
    Version                 =  4    ,
    Id                      =  5    ,
    Baud                    =  6    ,
    ReturnDelayTime         =  7    ,
    CwAngleLimit            =  8    ,
    CcwAngleLimit           =  9    ,
    ControlMode             = 10    ,
    LimitTemperature        = 11    ,
    DownLimitVoltage        = 12    ,
    UpLimitVoltage          = 13    ,
    MaxTorque               = 14    ,
    ReturnLevel             = 15    ,
    AlarmShutdown           = 16    ,

    TorqueEnable            = 17    ,
    Led                     = 18    ,
    Dgain                   = 19    ,
    Igain                   = 20    ,
    Pgain                   = 21    ,
    GoalPosition            = 22    ,
    GoalSpeed               = 23    ,
    GoalTorque              = 24    ,
    PresentPosition         = 25    ,
    PresentSpeed            = 26    ,
    PresentLoad             = 27    ,
    PresentVoltage          = 28    ,
    PresentTemperature      = 29    ,
    RegisteredInstruction   = 30    ,
    Moving                  = 31    ,
    HardwareError           = 32    ,
    Punch                   = 33    ,

    Total                   = 34    ,
};

//! Stored data about each entry of the control table
struct CmdInfo
{
    const char* name; // Name of the command
    RegIndex    areg; // Associated register in the servo
};

//! 
class Machine
{

    //! Table with cmd info
    static const CmdInfo CmdInfoTable[CmdIndex::Total];

    //! Maximal number of servo xl-320 selectable at once
    static constexpr byte CmdBufferSize = 128;

    //! Stream to read commands and write status
    Stream* mStream;

    // Buffer to store input command
    // Pointer on the next case of the buffer that can be filled
    char* mCmdPtr;
    char  mCmdBuffer[CmdBufferSize];

    //! Controller for xl-320 servo chain
    xl320::Controller mController;

public:

    //! Constructor
    Machine();

    //! Initialize the machine with a stream to read command and write status
    void setup(Stream& cmdStream, xl320::Controller::XlSerial& xlSerial);

    //! Check if there are some commands in the stream
    void loop();

private:

    //! Command parsing
    void parse(const char* command);

    //
    void parse_command(const char* ptr);
    void parse_command_getter();
    void parse_command_setter();

};


} // xlcmd

#endif // XLCMD_HPP
