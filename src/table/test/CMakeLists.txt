add_executable(footer_test footer_test.cpp)
target_link_libraries( footer_test
        ${GTEST_BOTH_LIBRARIES} 
        pthread
        zkv_table)
add_executable(table_builder_test table_builder_test.cpp)
target_link_libraries( table_builder_test
        ${GTEST_BOTH_LIBRARIES} 
        pthread
        zkv_file
        zkv_bloom
        zkv_table)
# gtest_discover_tests(allocTest)