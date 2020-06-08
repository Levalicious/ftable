cmake_minimum_required(VERSION 3.16)
project(ftable C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-O3")

add_executable(ftable main.c ftable.h)

target_link_libraries(ftable -lm)