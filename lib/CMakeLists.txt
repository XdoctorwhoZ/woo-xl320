CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)
PROJECT(woo-arduino-xl320 CXX)
MESSAGE("[ in] ${CMAKE_CURRENT_LIST_FILE}")


add_definitions(-DTEST)


# QT5_WRAP_CPP( MOCS_HDRS ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/Service.h )


# - Library
ADD_LIBRARY(
    woo-arduino-xl320
    SHARED
    Service.cxx
    ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/Service.h
   # ${MOCS_HDRS}
    )


QT5_USE_MODULES(
    woo-arduino-xl320
    Core SerialPort
    )

    # Controller.cxx

MESSAGE("[out] ${CMAKE_CURRENT_LIST_FILE}")
