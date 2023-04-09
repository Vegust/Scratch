cmake_minimum_required(VERSION 3.25)
project(Scratch)

set(CMAKE_CXX_STANDARD 20)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-c++98-compat)

include_directories(Source)

add_executable(Scratch WIN32
        Source/windows_entry.cpp)