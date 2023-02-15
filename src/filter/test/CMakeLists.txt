add_executable(bloomfilter_test bloomfilter_test.cpp)
target_link_libraries( bloomfilter_test
        ${GTEST_BOTH_LIBRARIES}
        pthread 
        zkv_bloom)
# gtest_discover_tests(allocTest)