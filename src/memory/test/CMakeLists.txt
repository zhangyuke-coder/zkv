add_executable(test_area area_test.cpp)
target_link_libraries( test_area
        ${GTEST_BOTH_LIBRARIES}
        pthread 
        zkv_memory)
# gtest_discover_tests(allocTest)