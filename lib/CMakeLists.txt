CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)
PROJECT(woo-arduino-xl320 CXX)
MESSAGE("[ in] ${CMAKE_CURRENT_LIST_FILE}")


ADD_DEFINITIONS(-DWOO_ARDUINO_XL320_DECL_EXPORT)



# - Library
ADD_LIBRARY(
    woo-arduino-xl320
    SHARED

	Packet.cxx
    Service.cxx
    SerialComDevice.cxx

    ${CMAKE_CURRENT_LIST_DIR}/../include/woo/xl320/Packet.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/woo/xl320/Service.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/woo/xl320/SerialComDevice.h

    )

# Command.cxx
# ServoIds.cxx
# ServoData.cxx
# Controller.cxx
# Service.cxx
# ServiceData.cxx
# ServiceSerial.cxx
# ServiceCommand.cxx
# ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/Command.h
# ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/Service.h
# ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/ServiceData.h
# ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/ServiceSerial.h
# ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/ServiceCommand.h
# ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/ServoIds.h
# ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/ServoData.h
# ${CMAKE_CURRENT_LIST_DIR}/../include/woo/arduino-xl320/Controller.h

QT5_USE_MODULES(
    woo-arduino-xl320
    Core SerialPort
    )


MESSAGE("[out] ${CMAKE_CURRENT_LIST_FILE}")
