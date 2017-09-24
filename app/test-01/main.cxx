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
    // std::lock_guard<std::mutex> lock(xmutex);
    cout << "    - Ping" << endl;
    xlService.sendPing();
}

//!
void testMachineState()
{
    // std::lock_guard<std::mutex> lock(xmutex);
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
            cout << "+ State 1" << endl;
            cout << "    - Id detected: " << xlService.getPingResult().size() << endl;
            if (xlService.getPingResult().size() > 0)
            {
                servo = xlService.getServo(xlService.getPingResult().front());
                servo->pull(woo::xl320::Servo::RegisterIndex::ID);
            }
            else
            {
                asioService.stop();
                cout << "    - No servo detected stop test" << endl;
            }
            break;   
        }
        case 2:
        {
            cout << "+ State 2" << endl;
            cout << *servo << endl;
            servo->pullAll();
            // servo->pull(woo::xl320::Servo::RegisterIndex::PresentPosition);
            break;
        }
        case 3:
        {
            cout << "+ State 3" << endl;
            cout << *servo << endl;
            cout << "    - Set goal pos to 0 then push" << endl;
            servo->set(woo::xl320::Servo::RegisterIndex::GoalPosition, 0);
            servo->push();
            break;
        }
        case 4:
        {
            cout << "+ State 4" << endl;
            cout << "    - Set goal pos to 1000 then push" << endl;
            servo->set(woo::xl320::Servo::RegisterIndex::GoalPosition, 1000);
            servo->push();
            break;   
        }
        case 5:
        {
            cout << "+ Test end" << endl;
            asioService.stop();
            break;
        }
    }
}

//
void onCommandFinish(woo::xl320::CommandExecutionStats stats)
{
    // std::lock_guard<std::mutex> lock(xmutex);
    state++;
    asioService.post( &testMachineState );
    // actionTimer.expires_from_now( boost::posix_time::seconds(0) );
    // actionTimer.async_wait( boost::bind(&testMachineState) );
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


    boost::asio::io_service::work idleWork(asioService);

    // Start actions

    asioService.post( &testMachineState );

    // actionTimer.expires_from_now( boost::posix_time::seconds(0) );
    // actionTimer.async_wait( boost::bind(&testMachineState) );

    // Start loop
    asioService.run();

    return 0;
}
