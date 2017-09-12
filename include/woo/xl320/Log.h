#ifndef WOO_XL320_LOG_H
#define WOO_XL320_LOG_H
#pragma once

// boost
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#define LOG_TRACE   BOOST_LOG_TRIVIAL(trace)
#define LOG_DEBUG   BOOST_LOG_TRIVIAL(debug)
#define LOG_INFO    BOOST_LOG_TRIVIAL(info) 
#define LOG_WARNING BOOST_LOG_TRIVIAL(warning)
#define LOG_ERROR   BOOST_LOG_TRIVIAL(error)
#define LOG_FATAL   BOOST_LOG_TRIVIAL(fatal)

#endif // WOO_XL320_LOG_H
