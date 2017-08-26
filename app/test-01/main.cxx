// Qt
#include <QDebug>
#include <QCoreApplication>

// woo
#include <woo/arduino-xl320/Service.h>
// #include <woo/arduino-xl320/Controller.h>

// std
#include <iostream>

// ---
using namespace std;

//! Main enter point
//!
int main(int argc, char *argv[])
{
    // Check if device is provided
    if( argc < 2 ) {
        cerr << "usage: test-01 /dev/tty" << endl;
        return 1;
    }

    // Start qt core application
    QCoreApplication a(argc, argv);

    // Create xl320 comminucation service
    woo::arduino_xl320::Service xservice;
    xservice.setDevName(QString(argv[1]));

    // Start this service
    if( xservice.start() ) {
        qDebug() << "Fail to start xl320 service";
        return 1;
    }

    // Send test
    QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);
    QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendPing);
    QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);
    QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);
    QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);
    QTimer::singleShot(2000, &xservice, &woo::arduino_xl320::Service::sendTest);





    return a.exec();
}

