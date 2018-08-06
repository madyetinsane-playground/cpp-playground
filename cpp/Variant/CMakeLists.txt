project(variant)

add_executable(variant main.cpp)

install(TARGETS variant RUNTIME DESTINATION bin)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++0x -Wall")
