// Qt
// #include <QCoreApplication>

#include <woo/xl320/Service.h>

// std
#include <iostream>

// ---
using namespace std;


woo::xl320::Service ser;
boost::asio::io_service iosss;

boost::asio::deadline_timer ttt(iosss);

void slotNewId(uint8_t id)
{
    LOG_INFO << "SLOT New ID: " << (int)id ;
}

//! Main enter point
//!
int main(int argc, char *argv[])
{
    // Check if device is provided
    // if( argc < 2 ) {
    //     cerr << "usage: test-01 /dev/tty" << endl;
    //     return 1;
    // }

    boost::log::core::get()->set_filter
    (
        // boost::log::trivial::severity >= boost::log::trivial::trace
        // boost::log::trivial::severity >= boost::log::trivial::debug
        boost::log::trivial::severity >= boost::log::trivial::info
    );

    ser.start();

    ser.newPingIdReceived.connect(&slotNewId);


    ttt.expires_from_now( boost::posix_time::seconds(2) );
    ttt.async_wait( boost::bind(&woo::xl320::Service::sendPing, &ser) );



    // LOG_INFO << "Number of id detected: " << ser.getPingResult().size();
    // if(ser.getPingResult().size() < 0)
    // {
    //     return 1;
    // }

    boost::asio::io_service::work ww(iosss);
    iosss.run();

    // woo::xl320::Servo* servo = ser.getServo(ser.getPingResult().front());

    // servo->pullAll();
    // boost::this_thread::sleep_for(boost::chrono::milliseconds(5000));
    
    // LOG_INFO << servo;

    return 0;
}

