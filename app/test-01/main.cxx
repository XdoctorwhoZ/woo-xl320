// woo
#include <woo/xl320/Service.h>

// std
#include <iostream>

// boost
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// ---
using namespace std;

// 
int state = 0;
std::mutex xmutex;
woo::xl320::Service xlService;
boost::asio::io_service asioService;
boost::asio::deadline_timer actionTimer(asioService);

woo::xl320::Servo* servo;

//!
void goPing()
{
    std::lock_guard<std::mutex> lock(xmutex);
    cout << "    - Ping" << endl;
    xlService.sendPing();
}

//!
void testMachineState()
{
    std::lock_guard<std::mutex> lock(xmutex);
    switch(state)
    {
        case 0:
        {
            cout << "+ State 0, ping in 2 seconds" << endl;
            actionTimer.expires_from_now( boost::posix_time::seconds(2) );
            actionTimer.async_wait( boost::bind(&goPing) );
            break;
        }
        case 1:
        {
            cout << "+ State 1";
            cout << "    - Id detected: " << xlService.getPingResult().size();
            // // if(xlService.getPingResult().size() > 0)
            // {
                // servo = xlService.getServo(xlService.getPingResult().front());
                // servo = xlService.getServo(1);
                // servo->pullAll();
            // }
            // else
            // {
            //     BOOST_LOG_TRIVIAL(error) << "-- Test end, no servo available";    
            // }
            break;   
        }
        case 2:
        {
            cout << "+ Test end";
            asioService.stop();
            break;
        }
    }
}

//
void onCommandFinish()
{
    std::lock_guard<std::mutex> lock(xmutex);
    state++;
    actionTimer.expires_from_now( boost::posix_time::seconds(0) );
    actionTimer.async_wait( boost::bind(&testMachineState) );
    std::cout << "+ End" << std::endl;
}

//
void slotNewId(uint8_t id)
{
    std::cout << "+ New id detected: " << (int)id << std::endl;
}

//! Main enter point
//!
int main(int argc, char *argv[])
{
    // Check if device is provided
    if( argc < 2 ) {
        cerr << "usage: test-01 /dev/tty" << endl;
        return 1;
    }

    // Prepare xl service
    xlService.setSerialDevice(argv[1]);
    xlService.setSerialBaudrate(115200);
    xlService.start();

    //
    xlService.commandEnded.connect(&onCommandFinish);
    xlService.newPingIdReceived.connect(&slotNewId);

    // Start actions
    actionTimer.expires_from_now( boost::posix_time::seconds(0) );
    actionTimer.async_wait( boost::bind(&testMachineState) );

    // Start loop
    boost::asio::io_service::work idleWork(asioService);
    asioService.run();

    return 0;
}
