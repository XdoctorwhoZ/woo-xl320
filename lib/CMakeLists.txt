CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)
PROJECT(woo-arduino-xl320 CXX)
MESSAGE("[ in] ${CMAKE_CURRENT_LIST_FILE}")


# - Library
ADD_LIBRARY(
    woo-arduino-xl320
    SHARED
    Service.cxx
    Controller.cxx
    )

MESSAGE("[out] ${CMAKE_CURRENT_LIST_FILE}")
