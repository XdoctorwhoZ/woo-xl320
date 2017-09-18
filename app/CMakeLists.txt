MESSAGE("[ in] ${CMAKE_CURRENT_LIST_FILE}")

ADD_EXECUTABLE(
    test-01
    test-01/main.cxx
    )

# ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

set(CMAKE_BUILD_TYPE Debug)

TARGET_LINK_LIBRARIES( test-01 LINK_PUBLIC ${Boost_LIBRARIES} -pthread
woo-xl320
    
	)

# TARGET_LINK_LIBRARIES(
#     test-01
#     )


# - end
MESSAGE("[out] ${CMAKE_CURRENT_LIST_FILE}")
