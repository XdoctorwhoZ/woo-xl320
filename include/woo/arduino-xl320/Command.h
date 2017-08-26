#ifndef WOO_XL320_COMMAND_H
#define WOO_XL320_COMMAND_H

// Qt
#include <QTimer>
#include <QQueue>
#include <QtSerialPort/QSerialPort>

// ---
namespace woo { namespace xl320 {

//! Class to manage command that must be send
class Command
{

public:

    //! Type for each command
    enum Type
    {
        None   ,
        Getter ,
        Setter ,
    };

    //! Available command names
    enum Name
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

        Test                    = 34    ,

        Total                   = 35    ,
    };

private:

    // Information about the command
    struct NameInfo
    {
        bool ronly;         //! True if the command is getter only
        const char* name;   //! String corresponding to Name enums
    };
    static const NameInfo NameInfos[Name::Total];

    //! Ids of servos that should get the command
    QString mIds;
    //! Command name
    Name    mName;
    //! Command to get or set value
    Type    mType;
    //! If type is setter, this is the value to set
    QString mValue;

public:

    //! Constructor
    Command ( const QString& ids = ""
            , Name name = Name::Test
            , Type type = Type::None
            , const QString& value = ""
            )
        : mIds(ids), mName(name), mType(type), mValue(value)
    { }

    //! Return true if the command needs some servos selection
    bool needIdSelection() const;

    //! Convert command information into a string that can be send to arduino
    QByteArray toData() const;

};

} // xl320
} // woo

#endif // WOO_XL320_COMMAND_H
