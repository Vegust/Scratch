#include "utils_windows.h"
#include "GLFW/glfw3.h"

float utils_windows::GetTime() {
	return static_cast<float>(glfwGetTime());
}
