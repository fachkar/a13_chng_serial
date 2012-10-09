cmake_minimum_required(VERSION 2.8)
project(a13bootnrecovery)
add_executable(a13bootnrecovery main.cpp)
find_library(LIBCURL curl)
target_link_libraries(a13bootnrecovery ${LIBCURL})

