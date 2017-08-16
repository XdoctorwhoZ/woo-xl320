#ifndef WOO_XL320_SERVICE_H
#define WOO_XL320_SERVICE_H

// #include <unistd.h>

// #include <iostream>

// Qt
#include <QTimer>
#include <QtSerialPort/QSerialPort>

#if defined TEST
// #error test
 #define TEST_COMMON_DLLSPEC Q_DECL_EXPORT
#else
// #error pastest
 #define TEST_COMMON_DLLSPEC Q_DECL_IMPORT
#endif


// ---
namespace woo { namespace xl320 {

// 

//! 
//!
class TEST_COMMON_DLLSPEC Service : public QObject
{
    Q_OBJECT

    //! Serial port device
    QString mDevName;

    // //! Mutex for the asio service thread
    // boost::mutex mMutex;

    // //! Asio service
    // boost::asio::io_service mIoService;
    
    //! Serial port controller
    QSerialPort* mSerialPort;

    // //! Flag to telle that \r has been received
    // bool rFlag;

    // //! Read buffer config
    // static constexpr int ReadBufRawSize = 512;
    // char mReadBufRaw[ReadBufRawSize];

    // //! Ready data buffer
    // std::string mReadBufStr;

    // boost::asio::streambuf bbbb;

    // boost::thread mIoServiceThread;

    // Port configuration
    QString                     mPortname;
    qint32                      mBaudrate;
    QSerialPort::DataBits       mDatabits;
    QSerialPort::StopBits       mStopbits;
    QSerialPort::Parity         mParity;
    QSerialPort::FlowControl    mFlowctrl;
    
    // Test control
    bool mTestResult;
    bool mIsTestRunnning;

public:

    Service(QObject* qparent = 0);
    // ~Service();

    void setDevName(const QString& dev) { mDevName = dev; }

    void start();
    // void stop();


    // To test communication with arduino
    void sendTest();
    bool isTestOver();
    bool getTestResult();


    void ping();
    // getController(int )


    // void asyncRead();
    // void onDataReceive(const boost::system::error_code& ec, size_t bytes_transferred);
    // void onDataReady(const std::string& data);

// private slots:


    void readData();



signals:

    //! Emitted when the result of the test is ready to be read
    //!
    void testResultReady(bool result);


};




} // xl320
} // woo

#endif // WOO_XL320_SERVICE_H
