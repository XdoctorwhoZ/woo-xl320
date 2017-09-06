#ifndef TEST01_H
#define TEST01_H
#pragma once

// Qt
#include <QDebug>
#include <QObject>

// woo
#include <woo/xl320/Servo.h>
#include <woo/xl320/Service.h>
#include <woo/xl320/SerialComDevice.h>

//!
//!
class Test01 : public QObject
{
    Q_OBJECT

    //! Current state of the state machine 
    int mState;

    //!
    woo::xl320::Service mService;

    //! 
    woo::xl320::SerialComDevice mSerialComDevice;

public:
    
    Test01()
    {

    }

    //!
    int start(const char* dev, qint32 baud = 115200)
    {
        int status;

        // Start serial device
        mSerialComDevice.setDevice(dev);
        mSerialComDevice.setBaudRate(baud);
        status = mSerialComDevice.start();
        if (status) {
            return status;
        }

        // Connect serial device with service
        connect(&mSerialComDevice, &woo::xl320::SerialComDevice::dataReceived   ,
                &mService        , &woo::xl320::Service::parseData              );
        connect(&mService        , &woo::xl320::Service::dataTxRequested    ,
                &mSerialComDevice, &woo::xl320::SerialComDevice::sendData   );




    //     // Reset state machine
    //     mState = 0;
    //     nextWork();

    //     // Get first id from PING

    //     // Get a controller for the first id

    //     // Request update for each of its data

    //     // when it is over

    //     // Send test
    //     // QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendPing);
    //     // QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);
    //     // QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);
    //     // QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);
    //     // QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);


        return 0;
    }

    //!
    //!
    // void nextWork()
    // {
    //     switch(mState)
    //     {
    //         case 0:
    //         {
    //             qDebug() << "====Send test in 2 sec";
    //             QTimer::singleShot(2000, &mService, SLOT(sendTest()));
    //         }
    //     }
    // }


};


#endif // TEST01_H
