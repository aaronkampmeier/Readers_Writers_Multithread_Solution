cmake_minimum_required(VERSION 3.10)
project(Project4 C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

set(CMAKE_C_STANDARD 99)

add_executable(Project4 proj-4.c)