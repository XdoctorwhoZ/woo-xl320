CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)
PROJECT(woo-arduino-xl320 CXX)
MESSAGE("[ in] ${CMAKE_CURRENT_LIST_FILE}")


add_definitions(-DTEST)



# - Library
ADD_LIBRARY(
    woo-arduino-xl320
    SHARED
    Command.cxx
    Service.cxx
    ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/Command.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/Service.h

    )


QT5_USE_MODULES(
    woo-arduino-xl320
    Core SerialPort
    )


MESSAGE("[out] ${CMAKE_CURRENT_LIST_FILE}")
