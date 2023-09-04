cmake_minimum_required(VERSION 3.26)
project(DLLInjector_main)

set(CMAKE_CXX_STANDARD 23)

add_executable(DLLInjector_main main.cpp
        inject.cpp
        inject.h)
