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
