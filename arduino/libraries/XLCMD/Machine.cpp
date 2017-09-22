// Woo
#include "XLCMD.h"

// ---
using namespace xlcmd;

// Uncomment to debug
#define XLCMDMachine_DEBUG

/* ============================================================================
 *
 * */
const CmdInfo Machine::CmdInfoTable[CmdIndex::Total] = {
    { "XBAUD"          , xl320::RegIndex::Total                  } , // XBaud                       
    { "PING"           , xl320::RegIndex::Total                  } , // Ping                        
    { "SEL"            , xl320::RegIndex::Total                  } , // Select                      
    { "MODEL"          , xl320::RegIndex::ModelNumber            } , // ModelNumber                 
    { "VERSION"        , xl320::RegIndex::Version                } , // Version                     
    { "ID"             , xl320::RegIndex::Id                     } , // Id                          
    { "BAUD"           , xl320::RegIndex::Baud                   } , // Baud                        
    { "RDT"            , xl320::RegIndex::ReturnDelayTime        } , // ReturnDelayTime             
    { "ANG_LIM_MIN"    , xl320::RegIndex::CwAngleLimit           } , // CwAngleLimit                
    { "ANG_LIM_MAX"    , xl320::RegIndex::CcwAngleLimit          } , // CcwAngleLimit               
    { "CTRL_MODE"      , xl320::RegIndex::ControlMode            } , // ControlMode                 
    { "TEMP_LIM"       , xl320::RegIndex::LimitTemperature       } , // LimitTemperature            
    { "VOLT_LIMIT_D"   , xl320::RegIndex::DownLimitVoltage       } , // DownLimitVoltage            
    { "VOLT_LIMIT_U"   , xl320::RegIndex::UpLimitVoltage         } , // UpLimitVoltage              
    { "MAX_TORQUE"     , xl320::RegIndex::MaxTorque              } , // MaxTorque                   
    { "RET_LEVEL"      , xl320::RegIndex::ReturnLevel            } , // ReturnLevel                 
    { "ALARM"          , xl320::RegIndex::AlarmShutdown          } , // AlarmShutdown               
    { "TORQUE_ENABLE"  , xl320::RegIndex::TorqueEnable           } , // TorqueEnable                
    { "LED"            , xl320::RegIndex::Led                    } , // Led                         
    { "DGAIN"          , xl320::RegIndex::Dgain                  } , // Dgain                       
    { "IGAIN"          , xl320::RegIndex::Igain                  } , // Igain                       
    { "PGAIN"          , xl320::RegIndex::Pgain                  } , // Pgain                       
    { "GPOS"           , xl320::RegIndex::GoalPosition           } , // GoalPosition                
    { "GSPEED"         , xl320::RegIndex::GoalSpeed              } , // GoalSpeed                   
    { "GTORQUE"        , xl320::RegIndex::GoalTorque             } , // GoalTorque                  
    { "PPOS"           , xl320::RegIndex::PresentPosition        } , // PresentPosition             
    { "PSPEED"         , xl320::RegIndex::PresentSpeed           } , // PresentSpeed                
    { "PLOAD"          , xl320::RegIndex::PresentLoad            } , // PresentLoad                 
    { "PVOLT"          , xl320::RegIndex::PresentVoltage         } , // PresentVoltage              
    { "PTEMP"          , xl320::RegIndex::PresentTemperature     } , // PresentTemperature          
    { "REG_INSTRU"     , xl320::RegIndex::RegisteredInstruction  } , // RegisteredInstruction       
    { "MOVING"         , xl320::RegIndex::Moving                 } , // Moving                      
    { "HARDWARE_ERROR" , xl320::RegIndex::HardwareError          } , // HardwareError               
    { "PUNCH"          , xl320::RegIndex::Punch                  } , // Punch                       
};

/* ============================================================================
 *
 * */
Machine::Machine()
{
    mCmdPtr = mCmdBuffer;
}

/* ============================================================================
 *
 * */
void Machine::setup(Stream& cmdStream, xl320::Controller::XlSerial& xlSerial)
{
    mStream = &cmdStream;
    mController.setup(xlSerial);
}

/* ============================================================================
 *
 * */
void Machine::loop()
{
    auto addCmdChar = [](char c)
    {
        if( (mCmdPtr-mCmdBuffer) < MaxCmdSize )
        {
            *mCmdPtr = c; mCmdPtr++;
        }
    };

    // Get input char and parse if needed
    while (mStream->available())
    {
        char c = mStream->read();
        addCmdChar(c);
        if (c=='\n')
        {
            addCmdChar('\0');
            parse(mCmdBuffer);
            rstCmdPtr();
        }
    }
}

/* ============================================================================
 *
 * */
void Machine::parse(const char* command)
{
    const char* ptr = command;

    // Check XL header
    if (*ptr != 'X') { syntaxError("Missing header char X."); return; }
    ptr++;
    if (*ptr != 'X') { syntaxError("Missing header char .L"); return; }
    ptr++;

    // Parse command
    if (*ptr == '+')
    {
        ptr++;
        parse_command(ptr);
    }
    else // case XL\r\n
    {
        if (*ptr != '\r') { syntaxError("Missing terminator \\r."); return; }
        ptr++;
        if (*ptr != '\n') { syntaxError("Missing terminator .\\n"); return; }
        ptr++;
        reply("OK\r\n");
    }
}

/* ============================================================================
 *
 * */
void Machine::parse_command(const char* ptr)
{
    // Get string limits
    const char* cmdbeg = ptr;
    while( *ptr != '\r' && *ptr != '?' && *ptr != '=' && *ptr != '\0' ) ptr++;

    char save = *ptr;
    *ptr = '\0';
    String stringTwo =  String("This is a string");

        // // Interpret string
        // XxCmd::Value cmd = XxCmd::GetValueFromName(cmdbeg, ptr);
        // if (cmd == XxCmd::Total) {
        //     syntaxError(); return;
        // }

        // // Check if this is a getter of setters function
        // if (*ptr != '?' && *ptr != '=') {
        //     syntaxError("need ? or ="); return;
        // }

        // // Getter command
        // if (*ptr == '?')
        // {
        //     // execute command XX+CMD?
        //     switch(cmd)
        //     {
        //         case XxCmd::XBaud                   : cmdXbaudGetter();                 break;
        //         case XxCmd::Ping                    : cmdPingGetter();                  break;
        //         case XxCmd::Select                  : cmdSelectGetter();                break;

        //         case XxCmd::ModelNumber             : cmdModelNumberGetter();           break;
        //         case XxCmd::Version                 : cmdVersionGetter();               break;
        //         case XxCmd::Id                      : cmdIdGetter();                    break;
        //         case XxCmd::Baud                    : cmdBaudGetter();                  break;
        //         case XxCmd::ReturnDelayTime         : cmdReturnDelayTimeGetter();       break;
        //         case XxCmd::CwAngleLimit            : cmdCwAngleLimitGetter();          break;
        //         case XxCmd::CcwAngleLimit           : cmdCcwAngleLimitGetter();         break;
        //         case XxCmd::ControlMode             : cmdControlModeGetter();           break;
        //         case XxCmd::LimitTemperature        : cmdLimitTemperatureGetter();      break;
        //         case XxCmd::DownLimitVoltage        : cmdDownLimitVoltageGetter();      break;
        //         case XxCmd::UpLimitVoltage          : cmdUpLimitVoltageGetter();        break;
        //         case XxCmd::MaxTorque               : cmdMaxTorqueGetter();             break;
        //         case XxCmd::ReturnLevel             : cmdReturnLevelGetter();           break;
        //         case XxCmd::AlarmShutdown           : cmdAlarmShutdownGetter();         break;
        //         case XxCmd::TorqueEnable            : cmdTorqueEnableGetter();          break;
        //         case XxCmd::Led                     : cmdLedGetter();                   break;
        //         case XxCmd::Dgain                   : cmdDgainGetter();                 break;
        //         case XxCmd::Igain                   : cmdIgainGetter();                 break;
        //         case XxCmd::Pgain                   : cmdPgainGetter();                 break;
        //         case XxCmd::GoalPosition            : cmdGoalPositionGetter();          break;
        //         case XxCmd::GoalSpeed               : cmdGoalSpeedGetter();             break;
        //         case XxCmd::GoalTorque              : cmdGoalTorqueGetter();            break;
        //         case XxCmd::PresentPosition         : cmdPresentPositionGetter();       break;
        //         case XxCmd::PresentSpeed            : cmdPresentSpeedGetter();          break;
        //         case XxCmd::PresentLoad             : cmdPresentLoadGetter();           break;
        //         case XxCmd::PresentVoltage          : cmdPresentVoltageGetter();        break;
        //         case XxCmd::PresentTemperature      : cmdPresentTemperatureGetter();    break;
        //         case XxCmd::RegisteredInstruction   : cmdRegisteredInstructionGetter(); break;
        //         case XxCmd::Moving                  : cmdMovingGetter();                break;
        //         case XxCmd::HardwareError           : cmdHardwareErrorGetter();         break;
        //         case XxCmd::Punch                   : cmdPunchGetter();                 break;

        //         default                             : syntaxError("unknown");
        //     }
        // }
        // // Setter command XX+CMD=...
        // else
        // {
        //     // skip '='
        //     ptr++;

        //     // execute command
        //     switch(cmd)
        //     {
        //         case XxCmd::XBaud                   : cmdXbaudSetter(ptr) ;                     break;
        //         case XxCmd::Select                  : cmdSelectSetter(ptr);                     break;

        //         case XxCmd::Id                      : cmdIdSetter(ptr);                         break;
        //         case XxCmd::Baud                    : cmdBaudSetter(ptr);                       break;
        //         case XxCmd::ReturnDelayTime         : cmdReturnDelayTimeSetter(ptr);            break;
        //         case XxCmd::CwAngleLimit            : cmdCwAngleLimitSetter(ptr);               break;
        //         case XxCmd::CcwAngleLimit           : cmdCcwAngleLimitSetter(ptr);              break;
        //         case XxCmd::ControlMode             : cmdControlModeSetter(ptr);                break;
        //         case XxCmd::LimitTemperature        : cmdLimitTemperatureSetter(ptr);           break;
        //         case XxCmd::DownLimitVoltage        : cmdDownLimitVoltageSetter(ptr);           break;
        //         case XxCmd::UpLimitVoltage          : cmdUpLimitVoltageSetter(ptr);             break;
        //         case XxCmd::MaxTorque               : cmdMaxTorqueSetter(ptr);                  break;
        //         case XxCmd::ReturnLevel             : cmdReturnLevelSetter(ptr);                break;
        //         case XxCmd::AlarmShutdown           : cmdAlarmShutdownSetter(ptr);              break;
        //         case XxCmd::TorqueEnable            : cmdTorqueEnableSetter(ptr);               break;
        //         case XxCmd::Led                     : cmdLedSetter(ptr);                        break;
        //         case XxCmd::Dgain                   : cmdDgainSetter(ptr);                      break;
        //         case XxCmd::Igain                   : cmdIgainSetter(ptr);                      break;
        //         case XxCmd::Pgain                   : cmdPgainSetter(ptr);                      break;
        //         case XxCmd::GoalPosition            : cmdGoalPositionSetter(ptr);               break;
        //         case XxCmd::GoalSpeed               : cmdGoalSpeedSetter(ptr);                  break;
        //         case XxCmd::GoalTorque              : cmdGoalTorqueSetter(ptr);                 break;
        //         case XxCmd::Punch                   : cmdPunchSetter(ptr);                      break;
        //         default                             : syntaxError("unknown");                   break;
        //     }
        // }
}

/* ============================================================================
 *
 * */
void Machine::parse_command_getter()
{

}

/* ============================================================================
 *
 * */
void Machine::parse_command_setter()
{

}
