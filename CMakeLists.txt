cmake_minimum_required(VERSION 3.25)
project(Scratch)
set(CMAKE_CXX_STANDARD 23)

include_directories(Source)
include_directories(Source/Core)

file(GLOB_RECURSE SCRATCH_CORE_SOURCES "Source/Core/*.cpp")
file(GLOB_RECURSE SCRATCH_APPLICATION_SOURCES "Source/Application/*.cpp")
file(GLOB_RECURSE SCRATCH_GAME_SOURCES "Source/Game/*.cpp")
file(GLOB_RECURSE SCRATCH_RENDERING_SOURCES "Source/Rendering/*.cpp")
file(GLOB_RECURSE SCRATCH_UI_SOURCES "Source/UI/*.cpp")
file(GLOB_RECURSE SCRATCH_RESOURCE_SOURCES "Source/Resource/*.cpp")
file(GLOB_RECURSE SCRATCH_SCENE_OBJECT_SOURCES "Source/SceneObjects/*.cpp")

set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
add_compile_definitions(NOMINMAX)
add_compile_definitions(WIN32_LEAN_AND_MEAN)

add_executable(Scratch
		Source/main.cpp
		${SCRATCH_CORE_SOURCES}
		${SCRATCH_APPLICATION_SOURCES}
		${SCRATCH_GAME_SOURCES}
		${SCRATCH_RENDERING_SOURCES}
		${SCRATCH_UI_SOURCES}
		${SCRATCH_RESOURCE_SOURCES}
		${SCRATCH_SCENE_OBJECT_SOURCES}
)

# ----------- Testing ------------
enable_testing()
include(CTest)
add_subdirectory(Testing)
# --------------------------------

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
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
add_subdirectory(Source/ThirdParty/assimp EXCLUDE_FROM_ALL)
include_directories(Source/ThirdParty/assimp/include)
target_link_libraries(Scratch assimp)
# --------------------------------