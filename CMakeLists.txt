cmake_minimum_required(VERSION 3.15)
project(Ex2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Ex2 uthreads.h uthreads.cpp scheduler.cpp scheduler.h thread.cpp thread.h utils.h main.cpp)
