
add_executable(file_name_test file_name_test.cpp)
target_link_libraries(file_name_test zkv_file)

add_executable(file_test file_test.cpp)
target_link_libraries(file_test zkv_file)