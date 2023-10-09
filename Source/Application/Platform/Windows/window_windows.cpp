#include "window_windows.h"
#include "Application/Input/input_state.h"
#include "Rendering/rendering_types.h"
#include "imgui.h"
#include "Rendering/OldRender/renderer.h"
#include "Application/application.h"

#include <GLFW/glfw3.h>

static application* GetApplication(struct GLFWwindow* Window) {
	return (application*) (glfwGetWindowUserPointer(Window));
}

static void OnWindowResize(GLFWwindow* Window, int NewWidth, int NewHeight) {
	if (application* App = GetApplication(Window)) {
		App->GetRenderer().OnScreenSizeChanged(NewWidth, NewHeight);
		App->TestMap.OnScreenSizeChanged(NewWidth, NewHeight);
	}
}

static void OnMouseMoved(GLFWwindow* Window, double XPos, double YPos) {
	static double LastXPos{XPos};
	static double LastYPos{YPos};

	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse) {
		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			if (application* App = GetApplication(Window)) {
				if (!App->GetRenderer().mCustomCamera.expired()) {
					auto CameraHandle = App->GetRenderer().mCustomCamera.lock();
					CameraHandle->OnMouseMoved(Window, XPos - LastXPos, YPos - LastYPos);
				}
			}
		}
	}

	LastXPos = XPos;
	LastYPos = YPos;
}

static void OnMouseScroll(struct GLFWwindow* Window, double XDelta, double YDelta) {
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse) {
		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			if (application* App = GetApplication(Window)) {
				if (!App->GetRenderer().mCustomCamera.expired()) {
					auto CameraHandle = App->GetRenderer().mCustomCamera.lock();
					CameraHandle->OnMouseScroll(Window, XDelta, YDelta);
				}
			}
		}
	}
}

void window_windows::Init(application* App, u32 WindowWidth, u32 WindowHeight) {
	glfwInit();
	mWindow = glfwCreateWindow(WindowWidth, WindowHeight, "Scratch", nullptr, nullptr);
	CHECK(mWindow)
	glfwSetWindowUserPointer(mWindow, App);
	SetVSync(true);

	glfwSetFramebufferSizeCallback(mWindow, OnWindowResize);
	glfwSetCursorPosCallback(mWindow, OnMouseMoved);
	glfwSetScrollCallback(mWindow, OnMouseScroll);
}

void window_windows::SwapBuffers() {
	glfwSwapBuffers(mWindow);
}

void window_windows::SetVSync(bool Enabled) {
	mVSync = Enabled;
	glfwSwapInterval(mVSync ? 1 : 0);
}

void* window_windows::GetApiLoadingFunction(rendering_api Api) {
	switch (Api) {
		case rendering_api::opengl:
			return (void*) (glfwGetProcAddress);
			break;
	}
	return nullptr;
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

void window_windows::ProcessEvents(input_state& InputState, renderer& Renderer, float DeltaTime) {
	glfwPollEvents();
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
		if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(mWindow, true);
		}
		if (!Renderer.mCustomCamera.expired()) {
			const bool CursorHeld =
				glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
			SetCursorEnabled(!CursorHeld);
			auto CameraHandle = Renderer.mCustomCamera.lock();
			CameraHandle->ProcessInput(mWindow, DeltaTime);
		}
	}
}

void window_windows::SetCursorEnabled(bool Enabled) {
	mCursorEnabled = Enabled;
	glfwSetInputMode(mWindow, GLFW_CURSOR, Enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}