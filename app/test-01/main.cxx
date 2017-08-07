// woo
// #include <woo/arduino-xl320/Service.h>
// #include <woo/arduino-xl320/Controller.h>


#include <iostream>

// ---
using namespace std;




int main(int argc, char *argv[])
{
    // Check if device is provided
    if( argc < 2 ) {
        cerr << "usage: test-01 /dev/tty" << endl;
        return 1;
    }

    // // Create device
    // woo::xl320::Service xservice;
    // xservice.setDevName(argv[1]);
    // xservice.start();

    // // boost::this_thread::sleep( boost::posix_time::seconds(2) );

    // // cout << "PING" << endl;
    // // xservice.ping();

    // boost::this_thread::sleep( boost::posix_time::seconds(5) );

    // // xservice.stop();


    return 0;
}

