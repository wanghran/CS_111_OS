cmake_minimum_required(VERSION 3.8)
project(PJ3A)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES lab3a.c ext2_fs.h)
add_executable(PJ3A ${SOURCE_FILES})