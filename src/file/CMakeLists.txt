
file(GLOB db_SRCS ./*.cpp)

add_library(zkv_file ${db_SRCS})
# target_link_libraries(muduo_base pthread rt)

#add_library(muduo_base_cpp11 ${base_SRCS})
#target_link_libraries(muduo_base_cpp11 pthread rt)
#set_target_properties(muduo_base_cpp11 PROPERTIES COMPILE_FLAGS "-std=c++0x")

install(TARGETS zkv_file DESTINATION lib)
#install(TARGETS muduo_base_cpp11 DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/muduo/base)
add_subdirectory(test)
if(MUDUO_BUILD_EXAMPLES)
  add_subdirectory(tests)
endif()