cmake_minimum_required(VERSION 3.2)

project(GUN_SOLDER)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall")

#设置cmake类型
set(CMAKE_BUILD_TYPE Debug)

include_directories(${CMAKE_SOURCE_DIR}/include )

add_executable(my_cmake_exe main.cpp src/Gun.cpp src/Solder.cpp)