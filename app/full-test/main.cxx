#include <iostream>



// class Controller
// {



// public:

//     Controller()
//     {

//     }

//     void start()
//     {

//     }

//     void stop()
//     {

//     }
    
// };

// } // xl320

#include <unistd.h>


#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>


using namespace::boost::asio; 

typedef boost::shared_ptr<boost::asio::serial_port> SerialPortPtr;

// using namespace xl320;

int main(int argc, char *argv[])
{
    boost::system::error_code ec;

    boost::asio::io_service mIoService;
    
    SerialPortPtr mPort;

    const char* com_port_name = "/dev/ttyACM0";

    mPort = SerialPortPtr(new boost::asio::serial_port(mIoService));
    mPort->open(com_port_name, ec);
    if (ec) {
        std::cout << "error : mPort->open() failed...com_port_name="
            << com_port_name << ", e=" << ec.message().c_str() << std::endl; 
        return false;
    }

    // option settings...
    mPort->set_option(boost::asio::serial_port_base::baud_rate(115200));
    // mPort->set_option(boost::asio::serial_port_base::character_size(8));
    mPort->set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    mPort->set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    mPort->set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));



    const char command[] = "\r\nXX+GPOS=0\r\n";

    std::size_t rr = write(*mPort, buffer(command, strlen(command)));
    std::cout << rr << std::endl;

    // std::size_t rr = mPort->write_some( boost::asio::buffer("\r\nXX+GPOS=0\r\n") );

    usleep(1000*1000*1);

    while(1)
    {
        try
        {
            char c[ strlen(command) + 1];
            read(*mPort, boost::asio::buffer(c, strlen(command) + 1));
            std::cout << c << std::endl;   
        }
        catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> >& e)
        {
            std::cout << "fail" << std::endl;
        }   
        usleep(1000*1000*1);
    }


    // mPort->close();


}

