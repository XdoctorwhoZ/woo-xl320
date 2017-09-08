CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)
PROJECT(woo-arduino-xl320 CXX)
MESSAGE("[ in] ${CMAKE_CURRENT_LIST_FILE}")

# 
ADD_DEFINITIONS(-DWOO_ARDUINO_XL320_DECL_EXPORT)

# - Library
ADD_LIBRARY(
    woo-arduino-xl320
    SHARED

    Servo.cxx
	Packet.cxx
    Service.cxx
    Command.cxx
    SerialComDevice.cxx

    ${CMAKE_CURRENT_LIST_DIR}/../include/woo/xl320/Servo.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/woo/xl320/Packet.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/woo/xl320/Service.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/woo/xl320/Command.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/woo/xl320/SerialComDevice.h

    )



QT5_USE_MODULES(
    woo-arduino-xl320
    Core SerialPort
    )


MESSAGE("[out] ${CMAKE_CURRENT_LIST_FILE}")
