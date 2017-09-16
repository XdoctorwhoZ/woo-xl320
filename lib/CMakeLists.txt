CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)
PROJECT(woo-xl320 CXX)
MESSAGE("[ in] ${CMAKE_CURRENT_LIST_FILE}")


# - Library
ADD_LIBRARY(
    woo-xl320
    SHARED
    Log.cxx
    Servo.cxx
    Packet.cxx
    Service.cxx
    Command.cxx
    )

TARGET_LINK_LIBRARIES( woo-xl320 LINK_PUBLIC ${Boost_LIBRARIES} -pthread)



MESSAGE("[out] ${CMAKE_CURRENT_LIST_FILE}")
