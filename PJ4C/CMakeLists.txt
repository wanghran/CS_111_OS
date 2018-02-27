cmake_minimum_required(VERSION 3.8)
project(PJ4C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES lab4c_tcp.c lab4c_tls.c)
add_executable(PJ4C ${SOURCE_FILES})