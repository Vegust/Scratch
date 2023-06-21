cmake_minimum_required(VERSION 3.25)
project(Scratch)
set(CMAKE_CXX_STANDARD 23)

# ------------ glfw --------------
add_subdirectory(Source/ThirdParty/glfw)
include_directories(Source/ThirdParty/glfw/include)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
link_libraries(glfw)
# --------------------------------

# not linking to default gl lib, using glad instead
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

# ------------ stb --------------
include_directories(Source/ThirdParty/stb)
add_library(stb STATIC Source/ThirdParty/stb/stb_image.cpp)
link_libraries(stb)
# --------------------------------

include_directories(Source)
include_directories(Source/Core)

add_compile_options(/GR- /GX-) # no exceptions, no typeinfo
add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-extra-semi-stmt)

add_executable(Scratch
		Source/main.cpp
		Source/Rendering/renderer.cpp
		Source/Rendering/vertex_buffer.cpp
		Source/Rendering/index_buffer.cpp
		Source/Rendering/vertex_array.cpp
		Source/Rendering/vertex_buffer_layout.cpp
		Source/Rendering/shader.cpp Source/Rendering/shader.h Source/Rendering/Texture.cpp)

