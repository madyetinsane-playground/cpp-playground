project(rendezvous)

add_executable(rendezvous main.cpp)
set(CMAKE_CXX_FLAGS "-pthread ${CMAKE_C_FLAGS} -std=c++0x -Wall")


install(TARGETS rendezvous RUNTIME DESTINATION bin)
