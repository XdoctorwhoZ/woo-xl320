// woo
#include <woo/xl320/Log.h>

// ---
using namespace woo::xl320;

/* ============================================================================
 *
 * */
std::mutex FileLog::Mutex;

/* ============================================================================
 *
 * */
ofStreamWrapper FileLog::File;

/* ============================================================================
 *
 * */
FileLog::FileLog(const char* func)
    : mLocalBuffer(new std::ostringstream())
{
    *mLocalBuffer << "[0x" << std::hex << std::this_thread::get_id() << "] - (" << func << ") ";
}

/* ============================================================================
 *
 * */
FileLog::~FileLog()
{
    std::lock_guard<std::mutex> lock(Mutex);
    File.openIfNot();
    File.stream << mLocalBuffer->str() << '\n';
    File.stream.flush();
}
