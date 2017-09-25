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

//! Define that must be used for debug logs in the module
#define WOO_XL320_LOG woo::xl320::log(__func__)

//! Wrapper to use the ofstream
class ofStreamWrapper
{
    //! Name of the log file
    static constexpr const char* FileName = "woo-xl320.log";

public:
    std::ofstream stream;
    ofStreamWrapper() { }
    ~ofStreamWrapper() { if(stream.is_open()) { stream.close(); } }
    void openIfNot() { if(!stream.is_open()) { stream.open(FileName); } }
};

//! Class to log into a debug file
class FileLog
{
    static std::mutex Mutex;
    static ofStreamWrapper File;
    std::shared_ptr<std::ostringstream> mLocalBuffer;

public:
    FileLog(const char* func = "");    
    ~FileLog();
    template<typename T>
    FileLog& operator<<(const T& el) { *mLocalBuffer << el; return *this; }
};

//! Empty log interface to skip logs
class EmptyLog
{
public:
    EmptyLog() { }
    template<typename T>
    EmptyLog& operator<<(const T& el) { return *this; }
};

//! Interface function for the all application
inline auto log(const char* func = "")
{
#ifdef WOO_XL320_DEBUG_LOG
    return FileLog(func);
#else
    return EmptyLog();
#endif
}

} // xl320
} // woo

#endif // WOO_XL320_LOG_H
