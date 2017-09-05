MESSAGE("[ in] ${CMAKE_CURRENT_LIST_FILE}")

ADD_EXECUTABLE(
    test-01
    test-01/main.cxx
    test-01/Test01.h
    )





TARGET_LINK_LIBRARIES(
    test-01
    woo-arduino-xl320
    )

QT5_USE_MODULES(
    test-01
    Core SerialPort
    )



MESSAGE("[out] ${CMAKE_CURRENT_LIST_FILE}")
