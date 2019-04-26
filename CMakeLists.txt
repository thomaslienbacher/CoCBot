cmake_minimum_required(VERSION 3.13)
project(CoCBot)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -msse2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(CoCBot src/main.cpp src/device.cpp src/device.hpp src/util.cpp src/util.hpp src/bot.cpp src/bot.hpp)

target_link_libraries(CoCBot tesseract lept)