#include "window_windows.h"

#ifdef WIN32

#include "Application/Input/input_state.h"
#include "Rendering/rendering_types.h"
#include "Rendering/OldRender/renderer.h"
#include "Application/application.h"
#include "glfw_keykodes_table.h"
#include <GLFW/glfw3.h>

static application* GetApplication(struct GLFWwindow* Window) {
	return (application*) (glfwGetWindowUserPointer(Window));
}

static void OnWindowResize(GLFWwindow* Window, int NewWidth, int NewHeight) {
	if (application* App = GetApplication(Window)) {
		App->GetRenderer().OnScreenSizeChanged(NewWidth, NewHeight);
		App->GetMap().OnScreenSizeChanged(NewWidth, NewHeight);
	}
}

static void OnMouseMoved(GLFWwindow* Window, double XPos, double YPos) {
	if (application* App = GetApplication(Window)) {
		auto& MouseData = App->GetInputState().GetMouseData();
		float DeltaX = static_cast<float>(XPos) - MouseData.mPosX;
		float DeltaY = static_cast<float>(YPos) - MouseData.mPosY;
		MouseData.mPosFrameDeltaX = static_cast<float>(DeltaX);
		MouseData.mPosFrameDeltaY = static_cast<float>(DeltaY);
		MouseData.mPosX = static_cast<float>(XPos);
		MouseData.mPosY = static_cast<float>(YPos);
	}
}

static void OnMouseScroll(GLFWwindow* Window, double XDelta, double YDelta) {
	if (application* App = GetApplication(Window)) {
		auto& MouseData = App->GetInputState().GetMouseData();
		MouseData.mScrollDeltaX = static_cast<float>(XDelta);
		MouseData.mScrollDeltaY = static_cast<float>(YDelta);
	}
}

static void OnKeyAction(GLFWwindow* Window, s32 GlfwKey, s32 Scancode, s32 Action, s32 ModsMask) {
	if (application* App = GetApplication(Window)) {
		input_key Key = GlfwButtonToInputKey[GlfwKey];
		key_state KeyState;
		if (Action == GLFW_PRESS || Action == GLFW_REPEAT) {
			KeyState = key_state::pressed;
		} else if (Action == GLFW_RELEASE) {
			KeyState = key_state::released;
		}
		App->GetInputState().SetKeyState(Key, KeyState);
	}
}

static void OnMouseAction(GLFWwindow* Window, s32 MouseButton, s32 Action, s32 ModsMask) {
	if (application* App = GetApplication(Window)) {
		input_key Key = GlfwMouseToInputKey[MouseButton];
		key_state KeyState;
		if (Action == GLFW_PRESS || Action == GLFW_REPEAT) {
			KeyState = key_state::pressed;
		} else if (Action == GLFW_RELEASE) {
			KeyState = key_state::released;
		}
		App->GetInputState().SetKeyState(Key, KeyState);
	}
}

void window_windows::Init(application* App, u32 WindowWidth, u32 WindowHeight) {
	glfwInit();
	mWindow = glfwCreateWindow((s32) WindowWidth, (s32) WindowHeight, "Scratch", nullptr, nullptr);
	CHECK(mWindow)
	glfwSetWindowUserPointer(mWindow, App);
	SetVSync(true);
	glfwSetFramebufferSizeCallback(mWindow, OnWindowResize);
	glfwSetCursorPosCallback(mWindow, OnMouseMoved);
	glfwSetScrollCallback(mWindow, OnMouseScroll);
	glfwSetKeyCallback(mWindow, OnKeyAction);
	glfwSetMouseButtonCallback(mWindow, OnMouseAction);
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

void window_windows::ProcessEvents(renderer& Renderer, float DeltaTime) {
	glfwPollEvents();
}

void window_windows::SetCursorEnabled(bool Enabled) {
	mCursorEnabled = Enabled;
	glfwSetInputMode(mWindow, GLFW_CURSOR, Enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

bool window_windows::GetCursorEnabled() const {
	return mCursorEnabled;
}

bool window_windows::GetVSync() const {
	return mVSync;
}

void window_windows::CloseWindow() {
	glfwSetWindowShouldClose(mWindow, true);
}

#endif