cmake_minimum_required(VERSION 3.26)
project(DLLInjector_main)

if(MSYS OR MINGW) # MINGW broken with current wxWidgets installation?
    add_link_options(-static -static-libgcc -static-libstdc++) # Fix missing library errors when running binary
elseif(MSVC)
    add_link_options(/SUBSYSTEM:WINDOWS)
endif()

set(CMAKE_CXX_STANDARD 23)

find_package(wxWidgets CONFIG REQUIRED)

add_executable(DLLInjector_main main.cpp
        inject.cpp
        inject.h
        gui.cpp
        gui.h
        setup.h)

target_link_libraries(DLLInjector_main PRIVATE wx::core wx::base)