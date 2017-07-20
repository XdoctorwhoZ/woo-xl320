// Woo
#include "XxCmd.hpp"

/* ============================================================================
 *
 * */
const char* XxCmd::Names[Value::Total] = {
    "XBAUD"             , // XBaud                       
    "PING"              , // Ping                        
    "SELECT"            , // Select                      

    "MODEL"             , // ModelNumber                 
    "VERSION"           , // Version                     
    "ID"                , // Id                          
    "BAUD"              , // Baud                        
    "RDT"               , // ReturnDelayTime             
    "ANG_LIM_MIN"       , // CwAngleLimit                
    "ANG_LIM_MAX"       , // CcwAngleLimit               
    "CTRL_MODE"         , // ControlMode                 
    "TEMP_LIM"          , // LimitTemperature            
    "VOLT_LIMIT_D"      , // DownLimitVoltage            
    "VOLT_LIMIT_U"      , // UpLimitVoltage              
    "MAX_TORQUE"        , // MaxTorque                   
    "RET_LEVEL"         , // ReturnLevel                 
    "ALARM"             , // AlarmShutdown               

    "TORQUE_ENABLE"     , // TorqueEnable                
    "LED"               , // Led                         
    "DGAIN"             , // Dgain                       
    "IGAIN"             , // Igain                       
    "PGAIN"             , // Pgain                       
    "GPOS"              , // GoalPosition                
    "GSPEED"            , // GoalSpeed                   
    "GTORQUE"           , // GoalTorque                  
    "PPOS"              , // PresentPosition             
    "PSPEED"            , // PresentSpeed                
    "PLOAD"             , // PresentLoad                 
    "PVOLT"             , // PresentVoltage              
    "PTEMP"             , // PresentTemperature          
    "REG_INSTRU"        , // RegisteredInstruction       
    "MOVING"            , // Moving                      
    "HARDWARE_ERROR"    , // HardwareError               
    "PUNCH"             , // Punch                       
};

/* ============================================================================
 *
 * */
XxCmd::Value XxCmd::GetValueFromName(const char* beg, const char* end)
{
    for(int i=0 ; i<Value::Total ; i++)
    {
        if (IsTheSameString(beg, end, Names[i]))
        {
            return (XxCmd::Value) i;
        }
    }
    return Value::Total;
}

/* ============================================================================
 *
 * */
bool XxCmd::IsTheSameString(const char* beg, const char* end, const char* word)
{
    int size0 = end - beg;
    int size1 = 0;
    const char* tmp = word;
    while(*tmp != '\0') { size1++; tmp++; }

    if( size0 != size1 )
    {
        return false;
    }

    while (beg != end)
    {
        if (*word == '\0') return false;
        if (*beg != *word) return false;

        word++;
        beg++;
    }
    return true;
}

