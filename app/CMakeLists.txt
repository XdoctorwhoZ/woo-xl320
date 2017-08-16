MESSAGE("[ in] ${CMAKE_CURRENT_LIST_FILE}")

ADD_EXECUTABLE(
    test-01
    test-01/main.cxx
    )





TARGET_LINK_LIBRARIES(
    test-01
    woo-arduino-xl320
    # Qt5::Core Qt5::SerialPort
    )

QT5_USE_MODULES(
    test-01
    Core SerialPort
    )



MESSAGE("[out] ${CMAKE_CURRENT_LIST_FILE}")
