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

# ------------ stb ---------------
include_directories(Source/ThirdParty/stb)
add_library(stb STATIC Source/ThirdParty/stb/stb_image.cpp)
link_libraries(stb)
# --------------------------------

# header only
# ------------ glm ---------------
include_directories(Source/ThirdParty/glm)
# --------------------------------

# --------- dear imgui -----------
include_directories(Source/ThirdParty/imgui)
add_library(imgui STATIC
		Source/ThirdParty/imgui/imgui.cpp
		Source/ThirdParty/imgui/imgui_demo.cpp
		Source/ThirdParty/imgui/imgui_draw.cpp
		Source/ThirdParty/imgui/imgui_widgets.cpp
		Source/ThirdParty/imgui/imgui_tables.cpp
		# opengl and glfw backends
		Source/ThirdParty/imgui/backends/imgui_impl_opengl3.cpp
		Source/ThirdParty/imgui/backends/imgui_impl_glfw.cpp
		)
link_libraries(imgui)
# --------------------------------

include_directories(Source)
include_directories(Source/Core)

add_compile_options(/GX-) # no exceptions
add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-extra-semi-stmt)

file(GLOB_RECURSE SCRATCH_RENDERING_SOURCES "Source/Rendering/*.cpp")
file(GLOB_RECURSE SCRATCH_TEST_SOURCES "Source/TestScenes/*.cpp")

add_executable(Scratch
		Source/main.cpp
		${SCRATCH_RENDERING_SOURCES}
		${SCRATCH_TEST_SOURCES}
		)

