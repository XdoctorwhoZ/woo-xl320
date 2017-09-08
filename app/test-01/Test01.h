#ifndef TEST01_H
#define TEST01_H
#pragma once

// Qt
#include <QList>
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

    //! 
    QList<woo::xl320::Servo> mServos;

public:
    
    Test01()
    { }

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
        connect(&mSerialComDevice, &woo::xl320::SerialComDevice::dataReceived           ,
                &mService        , &woo::xl320::Service::receiveData                    , Qt::DirectConnection );
        connect(&mService        , &woo::xl320::Service::commandTransmissionRequested   ,
                &mSerialComDevice, &woo::xl320::SerialComDevice::sendData               , Qt::DirectConnection );

        // Reset state machine
        mState = 0;
        QTimer::singleShot(0, this, SLOT(nextWork()));
        return 0;
    }

    //!
    void nextWork()
    {
        switch(mState)
        {
            case 0:
            {
                qDebug() << "# Send ping in 2 sec";
                QTimer::singleShot(2000, &mService, SLOT(sendPing()));
                mState++;
            }
            case 1:
            {
                QList<uint8_t> ids = mService.getPingResult();
                qDebug() << "# Found " << ids.size() << "ids";
                
                mServos.resize(ids.size());
                for(int i=0 ; i<ids.size() ; i++)
                {
                    mServos[i].setId(ids[i]);
                    // mService
                }

                mState++;
            }
        }
        // Restart state machine
        QTimer::singleShot(0, this, SLOT(nextWork()));
    }


};


#endif // TEST01_H
