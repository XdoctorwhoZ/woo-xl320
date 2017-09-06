#ifndef WOO_XL320_SERVICE_H
#define WOO_XL320_SERVICE_H

// Qt
#include <QTimer>
#include <QQueue>

// woo
#include "DllSpec.h"
#include "Command.h"

// ---
namespace woo { namespace xl320 {

//! Main class of arduino xl320
//! Control reception and transmission of data
//!
class Service : public QObject
{
    Q_OBJECT

public:

    //! Constructor
    Service(QObject* qparent = 0);

    //! Destructor
    ~Service();



    // //! Function to send a command to arduino
    // void sendCmd( const QString& ids = ""
    //             , Command::Name name = Command::Name::Test
    //             , Command::Type type = Command::Type::None
    //             , const QString& value = "")
    // { mServiceCommand.registerCommand(ids, name, type, value); }

    // //! Create a controller for the given servos ids
    // Controller getController(const QList<int>& ids);



    // Basic getters
    // bool getTestResult() const { return mResult.test; }
    // QList<int> getPingResult() const { return mResult.ping; }


public slots:

    //! Parse data from device com
    void parseData(const QByteArray& data);


    // //! To send test request
    // void sendTest();

    // //! To send a ping
    // void sendPing();

signals:

    
    //!
    void dataTxRequested(const QByteArray& data);

    // //! Emitted when the running command has received an answers or timeout
    // //!
    // void commandFinish();

    // //! Emitted when the result of the test is ready to be read
    // //!
    // void testResultReady(bool result);

};

} // xl320
} // woo

#endif // WOO_XL320_SERVICE_H
