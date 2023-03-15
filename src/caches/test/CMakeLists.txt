add_executable(cache_test cache_test.cpp)
target_link_libraries(cache_test zkv_memory zkv_util zkv_db)

# add_executable(wlru_test wlru_test.cpp)
# target_link_libraries(wlru_test zkv_memory zkv_util zkv_db)

# add_executable(wslru_test wslru_test.cpp)
# target_link_libraries(wslru_test zkv_memory zkv_util zkv_db)