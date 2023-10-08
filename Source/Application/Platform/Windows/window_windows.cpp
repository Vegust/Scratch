#include "window_windows.h"

#include <GLFW/glfw3.h>


void window_windows::Init(application* App, u32 WindowWidth, u32 WindowHeight) {
	glfwInit();
	mWindow = glfwCreateWindow(WindowWidth, WindowHeight, "Scratch", nullptr, nullptr);
	CHECK(mWindow)
	glfwSetWindowUserPointer(mWindow, App);
}

void window_windows::SwapBuffers() {
	glfwSwapBuffers(mWindow);
}

void window_windows::SetVSync(bool Enabled) {
	glfwSwapInterval(Enabled ? 1 : 0);
}

void* window_windows::GetApiLoadingFunction(rendering_api Api) {
	switch (Api) {
		case rendering_api::opengl:
			return (void*)(glfwGetProcAddress);
			break;
	}
	return nullptr;
}

application* window_windows::GetApplication() {
	return (application*)(glfwGetWindowUserPointer(mWindow));
}

application* window_windows::GetApplication(struct GLFWwindow* Window) {
	return (application*)(glfwGetWindowUserPointer(Window));
}

void window_windows::SetContextCurrent() {
	glfwMakeContextCurrent(mWindow);
}

window_windows::~window_windows() {
	glfwTerminate();
}

bool window_windows::ShouldClose() {
	return glfwWindowShouldClose(mWindow);
}

void window_windows::ProcessEvents() {
	glfwPollEvents();
//	ImGuiIO& io = ImGui::GetIO();
//	if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
//		if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//			glfwSetWindowShouldClose(Window, true);
//		}
//		if (!renderer::Get().mCustomCamera.expired()) {
//			if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
//				glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//			} else {
//				glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//			}
//			auto CameraHandle = renderer::Get().mCustomCamera.lock();
//			CameraHandle->ProcessInput(Window, DeltaTime);
//		}
//	}
}
