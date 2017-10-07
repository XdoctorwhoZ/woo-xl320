MESSAGE("[ in] ${CMAKE_CURRENT_LIST_FILE}")

# - Library woo-xl320
ADD_LIBRARY(
    WooXl320
    SHARED
    Log.cxx
    Order.cxx
    Servo.cxx
    Packet.cxx
    Service.cxx
    )
TARGET_LINK_LIBRARIES(
    WooXl320
    LINK_PUBLIC ${Boost_LIBRARIES} -pthread
    )

# - Install woo-xl320
INSTALL(
    TARGETS     WooXl320
    DESTINATION ${WOO_INSTALL_DIR_LIB}
    )

MESSAGE("[out] ${CMAKE_CURRENT_LIST_FILE}")
