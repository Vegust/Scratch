#include "platform_windows.h"
#include "GLFW/glfw3.h"

float platform_windows::GetTime() {
	return static_cast<float>(glfwGetTime());
}
