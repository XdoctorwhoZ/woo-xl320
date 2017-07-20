#ifndef XXCMD_HPP
#define XXCMD_HPP
#pragma once

//! XX Commands
//!
struct XxCmd
{
    //! Available XX commands
    //!
    enum Value
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

    //! Names for each Value
    //!
    static const char* Names[Value::Total];

    //!
    //!
    static Value GetValueFromName(const char* beg, const char* end);

private:

    //!
    //!
    static bool IsTheSameString(const char* beg, const char* end, const char* word);

};

#endif // XXCMD_HPP
