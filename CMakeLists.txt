cmake_minimum_required(VERSION 3.25)
project(Scratch)
set(CMAKE_CXX_STANDARD 23)

include_directories(Source)
include_directories(Source/Core)

file(GLOB_RECURSE SCRATCH_RENDERING_SOURCES "Source/Rendering/*.cpp")
file(GLOB_RECURSE SCRATCH_TEST_SCENES_SOURCES "Source/TestScenes/*.cpp")
file(GLOB_RECURSE SCRATCH_SCENE_OBJECT_SOURCES "Source/SceneObjects/*.cpp")

add_executable(Scratch
		Source/main.cpp
		${SCRATCH_RENDERING_SOURCES}
		${SCRATCH_TEST_SCENES_SOURCES}
		${SCRATCH_SCENE_OBJECT_SOURCES}
		)

target_compile_options(Scratch PRIVATE)
#target_compile_options(Scratch PRIVATE -Wno-unused-function -Wno-unused-variable)
#target_compile_options(Scratch PRIVATE /GX)
#target_compile_options(Scratch PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-extra-semi-stmt)

# ------------ glfw --------------
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(Source/ThirdParty/glfw)
include_directories(Source/ThirdParty/glfw/include)
target_link_libraries(Scratch glfw)
# --------------------------------

# ------------ glad --------------
include_directories(Source/ThirdParty/glad/include)
add_library(glad STATIC Source/ThirdParty/glad/src/glad.c)
target_link_libraries(Scratch glad)
# --------------------------------

# ------------ stb ---------------
include_directories(Source/ThirdParty/stb)
add_library(stb STATIC Source/ThirdParty/stb/stb_image.cpp)
target_link_libraries(Scratch stb)
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
target_link_libraries(Scratch imgui)
# --------------------------------

# ----------- assimp -------------
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_INSTALL OFF CACHE BOOL "")
add_subdirectory(Source/ThirdParty/assimp EXCLUDE_FROM_ALL)
include_directories(Source/ThirdParty/assimp/include)
target_link_libraries(Scratch assimp)
# --------------------------------