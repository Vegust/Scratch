cmake_minimum_required(VERSION 3.25)
project(Scratch)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-c++98-compat)

include_directories(Source)
include_directories(Source/Core)

add_executable(Scratch WIN32 Source/main.cpp Source/Core/app.cpp)