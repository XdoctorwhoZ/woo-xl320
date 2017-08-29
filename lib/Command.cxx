// woo
#include <woo/arduino-xl320/Command.h>

// std
#include <stdexcept>

// ---
using namespace woo::arduino_xl320;

/* ============================================================================
 *
 * */
const Command::NameInfo Command::NameInfos[Command::Name::Total] = {
    { false, "XBAUD"             } , // XBaud
    { false, "PING"              } , // Ping
    { false, "SEL"               } , // Select

    { true , "MODEL"             } , // ModelNumber
    { true , "VERSION"           } , // Version
    { false, "ID"                } , // Id
    { false, "BAUD"              } , // Baud
    { false, "RDT"               } , // ReturnDelayTime
    { false, "ANG_LIM_MIN"       } , // CwAngleLimit
    { false, "ANG_LIM_MAX"       } , // CcwAngleLimit
    { false, "CTRL_MODE"         } , // ControlMode
    { false, "TEMP_LIM"          } , // LimitTemperature
    { false, "VOLT_LIMIT_D"      } , // DownLimitVoltage
    { false, "VOLT_LIMIT_U"      } , // UpLimitVoltage
    { false, "MAX_TORQUE"        } , // MaxTorque
    { false, "RET_LEVEL"         } , // ReturnLevel
    { true , "ALARM"             } , // AlarmShutdown

    { false, "TORQUE_ENABLE"     } , // TorqueEnable
    { false, "LED"               } , // Led
    { false, "DGAIN"             } , // Dgain
    { false, "IGAIN"             } , // Igain
    { false, "PGAIN"             } , // Pgain
    { false, "GPOS"              } , // GoalPosition
    { false, "GSPEED"            } , // GoalSpeed
    { false, "GTORQUE"           } , // GoalTorque
    { true , "PPOS"              } , // PresentPosition
    { true , "PSPEED"            } , // PresentSpeed
    { true , "PLOAD"             } , // PresentLoad
    { true , "PVOLT"             } , // PresentVoltage
    { true , "PTEMP"             } , // PresentTemperature
    { true , "REG_INSTRU"        } , // RegisteredInstruction
    { true , "MOVING"            } , // Moving
    { true , "HARDWARE_ERROR"    } , // HardwareError
    { true , "PUNCH"             } , // Punch

    { true , ""                  } , // Test
};

/* ============================================================================
 *
 * */
Command::Name Command::NameStr2Id(const QByteArray& str)
{
    for(int i=0 ; i<Command::Name::Total ; i++)
    {
        const auto& info = Command::NameInfos[i];
        if( str == QByteArray(info.name) ) {
            return (Name)i;
        }
    }
    return Name::Total;
}

/* ============================================================================
 *
 * */
bool Command::needIdSelection() const
{
    QList<Name> noIdRequired = QList<Name>()
        << Name::Test
        << Name::XBaud
        << Name::Ping  
        << Name::Select;
    if( noIdRequired.indexOf(mName) == -1 ) { return true; }
    return false;
}

/* ============================================================================
 *
 * */
QByteArray Command::toData() const
{
    QByteArray cmd = "XX";
    if ( mName != Name::Test )
    {
        cmd += '+';
        cmd += NameInfos[mName].name;
        switch(mType)
        {
            case Type::Getter:
            {
                cmd += "?";
                break;
            }
            case Type::Setter:
            {
                if( NameInfos[mName].ronly )
                {
                    throw std::invalid_argument("Read-Only Command");
                }
                else
                {
                    cmd += "=" + mValue;
                }
                break;
            }
            default:
            {
                throw std::invalid_argument("Wrong Command Type");
                break;
            }
        }
    }
    cmd += "\r\n";
    return cmd;
}
