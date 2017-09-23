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
    { "BAUD"           , xl320::RegIndex::BaudRate               } , // BaudRate                        
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
    mController.setXlBaudRate(115200);
}

/* ============================================================================
 *
 * */
void Machine::loop()
{
    auto addCmdChar = [this](char c)
    {
        if( (mCmdPtr-mCmdBuffer) < CmdBufferSize )
        {
            *mCmdPtr = c; mCmdPtr++;
        }
    };

    auto rstCmdPtr = [this]()
    {
        mCmdPtr = mCmdBuffer;
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
void Machine::parse(char* command)
{
    char* ptr = command;

    // Check XL header
    if (*ptr != 'X') { reply("Missing header char X.\r\n"); return; }
    ptr++;
    if (*ptr != 'L') { reply("Missing header char .L\r\n"); return; }
    ptr++;

    // Parse command
    if (*ptr == '+')
    {
        ptr++;
        parse_command(ptr);
    }
    else // case XL\r\n
    {
        if (*ptr != '\r') { reply("Missing terminator \\r.\r\n"); return; }
        ptr++;
        if (*ptr != '\n') { reply("Missing terminator .\\n\r\n"); return; }
        ptr++;
        reply("OK\r\n");
    }
}

/* ============================================================================
 *
 * */
void Machine::parse_command(char* ptr)
{
    // Get string limits
    const char* cmdbeg = ptr;
    while( *ptr != '\r' && *ptr != '?' && *ptr != '=' && *ptr != '\0' ) ptr++;

    char save = *ptr;
    *ptr = '\0';
    String cmd_str =  String(cmdbeg);
    ptr++;
    
    CmdIndex index;
    for (uint16_t i=0 ; i<CmdIndex::Total ; i++)
    {
        if(cmd_str == CmdInfoTable[i].name)
        {
            index = (CmdIndex)i;
        }
    }

    // Check if this is a getter of setters function
    if (save != '?' && save != '=')
    {
        reply("Missing indicator ? or ="); return;
    }

    // Getter command
    if (save == '?')
    {
        parse_command_getter(ptr, index);
    }
    else
    {
        // parse_command_setter(ptr);
    }

}

/* ============================================================================
 *
 * */
void Machine::parse_command_getter(char* ptr, CmdIndex index)
{
    switch(index)
    {
        case XBaud:
        {

            String ret = "xbaud={";
            

            ret += "}\r\n";
            reply(ret.c_str());
            break;
        }
        case Ping:
        {
            uint8_t servoIds[xl320::Controller::SelectSizeMax];
            uint8_t numberOfServo; 
            numberOfServo = mController.ping(servoIds);
            String ret = "ping={";
            for(uint8_t i=0 ; i<numberOfServo ; i++)
            {
                if(i!=0) { ret += ", "; }
                ret += servoIds[i];
            }
            ret += "}\r\n";
            reply(ret.c_str());
            break;
        }
        case Select:
        {
            String ret = "select={";
            for(uint8_t i=0 ; i<mController.getSelectSize() ; i++)
            {
                if(i!=0) { ret += ", "; }
                ret += (int)mController.getSelectIds()[i];
            }
            ret += "}\r\n";
            reply(ret.c_str());
            break;
        }
        default:
        {
            String ret = CmdInfoTable[index].name;
            ret += "={";
            uint16_t read_value[xl320::Controller::SelectSizeMax];
            uint8_t nb = mController.pull(CmdInfoTable[index].areg, read_value);
            for(uint8_t i=0 ; i<mController.getSelectSize() ; i++)
            {
                if(i!=0) { ret += ", "; }
                if( read_value[i] != 0xFFFF )
                    ret += (int)read_value[i];
                else
                    ret += "???";
            }
            ret += "}\r\n";
            reply(ret.c_str());
            break;
        }
    }
}

/* ============================================================================
 *
 * */
void Machine::parse_command_setter(char* ptr, CmdIndex index)
{

}
