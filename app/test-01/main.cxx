// woo
#include <woo/arduino-xl320/Service.h>
// #include <woo/arduino-xl320/Controller.h>


#include <iostream>

#include <QCoreApplication>

// ---
using namespace std;




int main(int argc, char *argv[])
{

    // Check if device is provided
    if( argc < 2 ) {
        cerr << "usage: test-01 /dev/tty" << endl;
        return 1;
    }

    QCoreApplication a(argc, argv);


    woo::xl320::Service xservice;
    xservice.setDevName( QString(argv[1]) );

    xservice.start();

    QTimer::singleShot(2000, &xservice, &woo::xl320::Service::ping );

    // xservice.ping();
    
    return a.exec();

    // // Create device
    // xservice.start();

    // // boost::this_thread::sleep( boost::posix_time::seconds(2) );

    // // cout << "PING" << endl;
    // // xservice.ping();

    // boost::this_thread::sleep( boost::posix_time::seconds(5) );

    // // xservice.stop();


    // return 0;
}

