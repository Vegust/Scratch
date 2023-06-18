cmake_minimum_required(VERSION 3.25)
project(Scratch)
set(CMAKE_CXX_STANDARD 20)

# ------------ glfw --------------
add_subdirectory(Source/ThirdParty/glfw)
include_directories(Source/ThirdParty/glfw/include)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
link_libraries(glfw)
# --------------------------------

# ------------ opengl ------------
# find_package(OpenGL REQUIRED)
# include_directories(${OPENGL_INCLUDE_DIR})
# link_libraries(${OPENGL_LIBRARY})
# --------------------------------

# ------------ glad --------------
include_directories(Source/ThirdParty/glad/include)
add_library(glad STATIC Source/ThirdParty/glad/src/glad.c)
link_libraries(glad)
# --------------------------------

include_directories(Source)
include_directories(Source/Core)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic)
add_executable(Scratch Source/main.cpp)

