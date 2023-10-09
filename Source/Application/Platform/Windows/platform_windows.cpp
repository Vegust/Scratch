#include "platform_windows.h"
#include "GLFW/glfw3.h"

#ifdef WIN32

float platform_windows::GetTime() {
	return static_cast<float>(glfwGetTime());
}

#endif