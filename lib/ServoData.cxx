// woo
#include <woo/arduino-xl320/ServoData.h>

// Qt
#include <QDebug>

// ---
using namespace woo::arduino_xl320;


Servo::RegisterEntry Servo::RegisterMap[] =
{
    { "EEPROM",  0, 2, "Model Number"           , ReadOnly  , 350        , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "EEPROM",  2, 1, "Version of Firmware"    , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "EEPROM",  3, 1, "ID"                     , ReadWrite , 1          , 0          , 252        } ,
    { "EEPROM",  4, 1, "Baud Rate"              , ReadWrite , 3          , 0          , 3          } ,
    { "EEPROM",  5, 1, "Return Delay Time"      , ReadWrite , 250        , 0          , 254        } ,
    { "EEPROM",  6, 2, "CW Angle Limit"         , ReadWrite , 0          , 0          , 1023       } ,
    { "EEPROM",  8, 2, "CCW Angle Limit"        , ReadWrite , 1023       , 0          , 1023       } ,
    { "EEPROM", 11, 1, "Control Mode"           , ReadWrite , 2          , 1          , 2          } ,
    { "EEPROM", 12, 1, "Limit Temperature"      , ReadWrite , 65         , 0          , 150        } ,
    { "EEPROM", 13, 1, "lower Limit Voltage"    , ReadWrite , 60         , 50         , 250        } ,
    { "EEPROM", 14, 1, "Upper Limit Voltage"    , ReadWrite , 90         , 50         , 250        } ,
    { "EEPROM", 15, 2, "Max Torque"             , ReadWrite , 1023       , 0          , 1023       } ,
    { "EEPROM", 17, 1, "Return Level"           , ReadWrite , 2          , 0          , 2          } ,
    { "EEPROM", 18, 1, "Alarm Shutdown"         , ReadWrite , 3          , 0          , 7          } ,
    
    { "RAM"   , 24, 1, "Torque Enable"          , ReadWrite , 0          , 0          , 1          } ,
    { "RAM"   , 25, 1, "LED"                    , ReadWrite , 0          , 0          , 7          } ,
    { "RAM"   , 27, 1, "D Gain"                 , ReadWrite , 0          , 0          , 254        } ,
    { "RAM"   , 28, 1, "I Gain"                 , ReadWrite , 0          , 0          , 254        } ,
    { "RAM"   , 29, 1, "P Gain"                 , ReadWrite , 32         , 0          , 254        } ,
    { "RAM"   , 30, 2, "Goal Position"          , ReadWrite , 0xFFFFFFFF , 0          , 1023       } ,
    { "RAM"   , 32, 2, "Moving Speed"           , ReadWrite , 0xFFFFFFFF , 0          , 2047       } ,
    { "RAM"   , 35, 2, "Torque Limit"           , ReadWrite , 0xFFFFFFFF , 0          , 1023       } ,
    { "RAM"   , 37, 2, "Present Position"       , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 39, 2, "Present Speed"          , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 41, 2, "Present Load"           , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 45, 1, "Present Voltage"        , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 46, 1, "Present Temperature"    , ReadOnly  , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 47, 1, "Registered Instruction" , ReadOnly  , 0          , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 49, 1, "Moving"                 , ReadOnly  , 0          , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 50, 1, "Hardware Error Status"  , ReadOnly  , 0          , 0xFFFFFFFF , 0xFFFFFFFF } ,
    { "RAM"   , 51, 2, "Punch"                  , ReadWrite , 32         , 0          , 1023       } ,
};

