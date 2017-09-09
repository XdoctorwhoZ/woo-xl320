#ifndef TEST01_H
#define TEST01_H
#pragma once

// Qt
#include <QDebug>
#include <QVector>
#include <QObject>

// woo
#include <woo/xl320/Servo.h>
#include <woo/xl320/Service.h>
#include <woo/xl320/SerialComDevice.h>

using namespace woo::xl320;

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
    QList<woo::xl320::Servo*> mServos;

public:
    
    Test01()
    { }

public slots:

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

        // Connect service with the test object
        connect(&mService        , &woo::xl320::Service::newPingIdReceived              ,
                this             , &Test01::onNewId                                     , Qt::DirectConnection );
        connect(&mService        , &woo::xl320::Service::commandEnded                   ,
                this             , &Test01::onCommandFinish                             , Qt::DirectConnection );

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
                break;
            }
            case 1:
            {
                QList<uint8_t> ids = mService.getPingResult();
                qDebug() << "# Found " << ids.size() << "ids";

                if (mServos.size()>0)
                {
                    qDebug() << "# Pull ModelNumber value";
                    mServos[0]->pull(Servo::RegisterIndex::ModelNumber);
                }
                else
                {
                    qDebug() << "# No servo available... stop test";
                }
                break;
            }
            case 2:
            {
                qDebug() << "# ModelNumber of servo" << mServos[0]->get(Servo::RegisterIndex::ModelNumber);
                mServos[0]->pullAll();
                break;
            }
            case 3:
            {
                qDebug() << "#" << mServos[0]->toString();
                break;
            }
        }
    }


    void onNewId(uint8_t id)
    {
        qDebug() << "# New id" << id;
        mServos << mService.getServo(id);
    }

    void onCommandFinish()
    {
        qDebug() << "# Command finish";
        mState++;
        QTimer::singleShot(0, this, SLOT(nextWork()));
    }

};

#endif // TEST01_H
