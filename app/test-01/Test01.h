#ifndef TEST01_H
#define TEST01_H
#pragma once

// Qt
#include <QDebug>
#include <QObject>

// woo
#include <woo/arduino-xl320/Service.h>

//!
//!
class Test01 : public QObject
{
    Q_OBJECT

    //! Current state of the state machine 
    int mState;

    //! Service to control arduino
    woo::arduino_xl320::Service mService;

public:
    
    Test01() { }

    //!
    //!
    int start(const char* dev)
    {
        // Create xl320 comminucation service
        mService.setDevName(QString(dev));

        // Start this service
        if ( mService.start() ) {
            qDebug() << "====Fail to start xl320 service";
            return 1;
        }

        // Reset state machine
        mState = 0;
        nextWork();

        // Get first id from PING

        // Get a controller for the first id

        // Request update for each of its data

        // when it is over

        // Send test
        // QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendPing);
        // QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);
        // QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);
        // QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);
        // QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);


        return 0;
    }

    //!
    //!
    void nextWork()
    {
        switch(mState)
        {
            case 0:
            {
                qDebug() << "====Send test in 2 sec";
                QTimer::singleShot(2000, &mService, SLOT(sendTest()));
            }
        }
    }


};


#endif // TEST01_H
