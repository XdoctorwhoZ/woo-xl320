// woo
#include <woo/arduino-xl320/Service.h>
// #include <woo/arduino-xl320/Controller.h>

// Qt
#include <QDebug>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
Service::Service(QObject* qparent)
    : QObject(qparent)
    , mSerialPort(new QSerialPort(this))
    , mIsTestRunnning(false)
{
    connect(mSerialPort, &QSerialPort::readyRead, this, &Service::readData);
}

/* ============================================================================
 *
 * */
// Service::~Service()
// {
//     // stop();
// }

/* ============================================================================
 *
 * */
void Service::start()
{
    // Set default configuration
    mPortname = mDevName;
    mBaudrate = 115200;
    mDatabits = QSerialPort::DataBits::Data8;
    mStopbits = QSerialPort::StopBits::OneStop;
    mParity   = QSerialPort::Parity::NoParity;
    mFlowctrl = QSerialPort::FlowControl::NoFlowControl;

    // Set serial configuration
    mSerialPort->setPortName   ( mPortname );
    mSerialPort->setBaudRate   ( mBaudrate );
    mSerialPort->setDataBits   ( mDatabits );
    mSerialPort->setStopBits   ( mStopbits );
    mSerialPort->setParity     ( mParity   );
    mSerialPort->setFlowControl( mFlowctrl );


    if (mSerialPort->open(QIODevice::ReadWrite)) {


        


    } else {
        // QMessageBox::critical(this, tr("Error"), serial->errorString());

        // showStatusMessage(tr("Open error"));
    }

}

/* ============================================================================
 *
 * */
void Service::sendTest()
{
    mIsTestRunnning = true;

    QByteArray command = "XX\r\n";

    qDebug() << "PING : " << command;

    mSerialPort->write(command);
}

/* ============================================================================
 *
 * */
bool Service::isTestOver()
{
    return mIsTestRunnning;
}

/* ============================================================================
 *
 * */
bool Service::getTestResult()
{

}


/* ============================================================================
 *
 * */
// void Service::stop()
// {

// }

/* ============================================================================
 *
 * */
void Service::ping()
{
    QByteArray command = "XX\r\nXX+PING?\r\n";

    qDebug() << "PING : " << command;

    mSerialPort->write(command);
}

/* ============================================================================
 *
 * */
void Service::readData()
{
    QByteArray data = mSerialPort->readAll();
    qDebug() << data;
}

/* ============================================================================
 *
 * */
// void Service::asyncRead()
// {
    // // Check if port is open
    // if (!mSerialPort.is_open()) {
    //     std::cerr << "Error : Serial port is not opened" << std::endl;
    //     return;
    // }


    // std::cout << "async read start" << std::endl;


    // async_read_until(
    //     mSerialPort,
    //     bbbb,
    //     '\n',
    //     boost::bind(
    //         &Service::onDataReceive,
    //         this, boost::asio::placeholders::error, 
    //         boost::asio::placeholders::bytes_transferred)
    // );


    // Start async read
    // mSerialPort.async_read_some( 
    //     boost::asio::buffer(mReadBufRaw, ReadBufRawSize),
    //     boost::bind(
    //         &Service::onDataReceive,
    //         this, boost::asio::placeholders::error, 
    //         boost::asio::placeholders::bytes_transferred));
// }

/* ============================================================================
 *
 * */
// void Service::onDataReceive(const boost::system::error_code& ec, size_t bytes_transferred)
// {
    // First lock
    // boost::mutex::scoped_lock look(mMutex);

    // mMutex.lock();

    // // Reception erro
    // if (ec)
    // {
    //     if(ec == boost::asio::error::eof)
    //     {
    //         std::cerr << "Warning : serial connection closed" << std::endl;
    //         // asyncRead();
    //     }
    //     else
    //     {
    //         std::cerr << "Warning : " << ec.message() << std::endl;
    //         asyncRead();
    //     }
    //     return;
    // }

    // // log
    // std::cout << "Data Received : " << bytes_transferred << std::endl;


    // std::string s( (std::istreambuf_iterator<char>(&bbbb)), std::istreambuf_iterator<char>() );
    // onDataReady(s);

    // mMutex.unlock();

    // // 
    // for (int i=0 ; i<bytes_transferred ; ++i)
    // {
    //     char c = mReadBufRaw[i];
    //     switch(c)
    //     {
    //         case '\r': {
    //             rFlag = true;
    //         }
    //         break;

    //         case '\n': {
    //             if(rFlag)
    //             {
    //                 this->onDataReady(mReadBufStr);
    //                 mReadBufStr.clear();
    //             }
    //         }
    //         break;
        
    //         default:
    //         {   
    //             mReadBufStr += c;
    //             rFlag = false;
    //         }
    //     }
    // }


    // std::cout << data << std::endl;

    //
    // asyncRead();
// }

/* ============================================================================
 *
 * */
// void Service::onDataReady(const std::string& data)
// {
//     std::cout << data << std::endl;
// }
