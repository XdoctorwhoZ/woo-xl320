// Qt
#include <QCoreApplication>

//
#include "Test01.h"

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

    // Start test
    Test01 testObj;
    if( testObj.start(argv[1]) ) {
        return 1;   
    }

    return a.exec();
}

