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
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;

// 
int state = 0;
woo::xl320::Service xlService;
boost::asio::io_service asioService;
boost::asio::deadline_timer actionTimer(asioService);

woo::xl320::Servo* servo;

// log config
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", woo::xl320::LogLevel)

//!
void testMachineState()
{
    switch(state)
    {
        case 0:
        {
            BOOST_LOG_TRIVIAL(info) << "-- State 0";
            actionTimer.expires_from_now( boost::posix_time::seconds(2) );
            actionTimer.async_wait( boost::bind(&woo::xl320::Service::sendPing, &xlService) );
            break;
        }
        case 1:
        {
            BOOST_LOG_TRIVIAL(info) << "-- State 1";
            BOOST_LOG_TRIVIAL(info) << "   + Id detected: " << xlService.getPingResult().size();
            // if(xlService.getPingResult().size() > 0)
            // {
                // servo = xlService.getServo(xlService.getPingResult().front());
                servo = xlService.getServo(1);
                servo->pullAll();
            // }
            // else
            // {
            //     BOOST_LOG_TRIVIAL(error) << "-- Test end, no servo available";    
            // }
            break;   
        }
    }
}

//
void onCommandFinish()
{
    BOOST_LOG_TRIVIAL(info) << "-- ok";

    state++;

    actionTimer.expires_from_now( boost::posix_time::seconds(0) );
    actionTimer.async_wait( boost::bind(&testMachineState) );
}

//
void slotNewId(uint8_t id)
{
    BOOST_LOG_TRIVIAL(info) << "    + New id detected: " << (int)id;
}

void initLogs()
{
    // // Prepare 
    // typedef boost::log::sinks::synchronous_sink< 
    //     boost::log::sinks::text_ostream_backend > text_sink;
    // boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

    // // sink->locked_backend()->add_stream(
    // //     boost::make_shared< std::ofstream >("full.log"));
    // sink->locked_backend()->add_stream(
    //     boost::shared_ptr<std::ostream>(&std::cout, [](std::ostream* d){} ));

    // sink->set_formatter
    // (
    //     expr::stream << expr::smessage
    // );

    // // sink->set_filter(
    // //     severity >= woo::xl320::info
    // //     );

    // logging::core::get()->add_sink(sink);
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

    initLogs();

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



    // boost::this_thread::sleep_for(boost::chrono::milliseconds(5000));
    
    // LOG_INFO << servo;


    boost::asio::io_service::work idleWork(asioService);
    asioService.run();

    return 0;
}

/*
    // // Prepare 
    // typedef boost::log::sinks::synchronous_sink< 
    //     boost::log::sinks::text_ostream_backend > text_sink;
    // boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

    // sink->locked_backend()->add_stream(
    //     boost::make_shared< std::ofstream >("full.log"));
    // sink->locked_backend()->add_stream(
    //     boost::shared_ptr<std::ostream>(&std::cout, [](std::ostream* d){} ));


    // sink->set_formatter
    // (
    //     expr::stream << "___"  << expr::smessage
    // );

    // sink->set_filter(channel == "woo::xl320");

    // logging::core::get()->add_sink(sink);
*/
