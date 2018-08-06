project(thread_pool)

add_executable(thread_pool main.cpp thread_pool.h)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -Wall")
target_link_libraries(thread_pool pthread)