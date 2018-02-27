cmake_minimum_required(VERSION 3.8)
project(PJ4B)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        example.c
        lab4b.c
        Makefile
        README)

add_executable(PJ4B ${SOURCE_FILES})