MESSAGE("[ in] ${CMAKE_CURRENT_LIST_FILE}")

# - Prepare test-01
ADD_EXECUTABLE(
    test-01
    test-01/main.cxx
    )
TARGET_LINK_LIBRARIES(
    test-01
    LINK_PUBLIC ${Boost_LIBRARIES} -pthread WooXl320
	)



# - end
MESSAGE("[out] ${CMAKE_CURRENT_LIST_FILE}")
