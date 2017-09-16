#ifndef WOO_XL320_LOG_H
#define WOO_XL320_LOG_H
#pragma once

// std
#include <mutex>
#include <thread>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>

// ---
namespace woo { namespace xl320 {

class ofStreamWrapper
{

public:

    std::ofstream stream;

    ofStreamWrapper(){}
    
    ~ofStreamWrapper()
    {
        if(stream.is_open()) { stream.close(); }
    }
    
    void openIfNot()
    {
        if(!stream.is_open()) { stream.open("woo_xl320.log"); }
    }
};

//!
class FileLog
{

    static std::mutex Mutex;
    static ofStreamWrapper File;

    std::shared_ptr<std::ostringstream> mLocalBuffer;

public:

    FileLog() : mLocalBuffer(new std::ostringstream()) { }
    
    ~FileLog()
    {
        std::lock_guard<std::mutex> lock(Mutex);
        File.openIfNot();
        File.stream
            << "0x" << std::hex << std::this_thread::get_id()
            << " # " << mLocalBuffer->str()
            << "\n";
        File.stream.flush();
    }

    template<typename T>
    FileLog& operator<<(const T& el)
    {
        *mLocalBuffer << el;
        return *this;
    }
};


class EmptyLog
{
public:
    EmptyLog() { }
    
    template<typename T>
    EmptyLog& operator<<(const T& el) { return *this; }
};


inline auto log()
{
// #if
    return FileLog();
    // return EmptyLog();
}

} // xl320
} // woo

#endif // WOO_XL320_LOG_H
