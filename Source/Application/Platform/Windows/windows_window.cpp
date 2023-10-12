#include "windows_window.h"

#ifdef WIN32

#include "Rendering/rendering_types.h"
#include "glfw_keykodes_table.h"
#include <GLFW/glfw3.h>

struct proc_data {
	window_process_result& mResult;
	frame_input_state& mFrameInput;
	windows_window& mWindow;
};

static proc_data* GetData(struct GLFWwindow* Window) {
	return (proc_data*) (glfwGetWindowUserPointer(Window));
}

static void OnWindowResize(GLFWwindow* Window, int NewWidth, int NewHeight) {
	proc_data* Data = GetData(Window);
	Data->mWindow.mState.mHeight = NewHeight;
	Data->mWindow.mState.mWidth = NewWidth;
	const auto Index = Data->mResult.mMessages.FindFirstByPredicate(
		[](const auto& Message) { return Message.mType == app_message_type::render_resize; });
	if (Index != InvalidIndex) {
		Data->mResult.mMessages[Index].mRenderResize.NewWidth = NewWidth;
		Data->mResult.mMessages[Index].mRenderResize.NewHeight = NewHeight;
	} else {
		app_message ResizeMessage;
		ResizeMessage.mType = app_message_type::render_resize;
		ResizeMessage.mRenderResize.NewWidth = NewWidth;
		ResizeMessage.mRenderResize.NewHeight = NewHeight;
		Data->mResult.mMessages.Add(ResizeMessage);
	}
}

static void OnMouseMoved(GLFWwindow* Window, double XPos, double YPos) {
	proc_data* Data = GetData(Window);
	auto& MouseState = Data->mFrameInput.mMouseState;
	MouseState.mPos = {XPos, YPos};
}

static void OnMouseScroll(GLFWwindow* Window, double XDelta, double YDelta) {
	proc_data* Data = GetData(Window);
	auto& MouseState = Data->mFrameInput.mMouseState;
	MouseState.mScroll = {XDelta, YDelta};
}

static void OnKeyChange(GLFWwindow* Window, input_key Key, s32 Action) {
	key_state KeyState;
	if (Action == GLFW_PRESS || Action == GLFW_REPEAT) {
		KeyState = key_state::pressed;
	} else if (Action == GLFW_RELEASE) {
		KeyState = key_state::released;
	}
	proc_data* Data = GetData(Window);
	Data->mFrameInput.mKeyStates[static_cast<u32>(Key)] = KeyState;
}

static void OnKeyAction(GLFWwindow* Window, s32 GlfwKey, s32 Scancode, s32 Action, s32 ModsMask) {
	input_key Key = GlfwButtonToInputKey[GlfwKey];
	OnKeyChange(Window, Key, Action);
}

static void OnMouseAction(GLFWwindow* Window, s32 MouseButton, s32 Action, s32 ModsMask) {
	input_key Key = GlfwMouseToInputKey[MouseButton];
	OnKeyChange(Window, Key, Action);
}

windows_window::windows_window(u32 WindowWidth, u32 WindowHeight) {
	mState.mHeight = WindowHeight;
	mState.mWidth = WindowWidth;
	glfwInit();
	mWindow = glfwCreateWindow((s32) WindowWidth, (s32) WindowHeight, "Scratch", nullptr, nullptr);
	CHECK(mWindow)
	glfwSwapInterval(mState.mVSync ? 1 : 0);
	glfwSetFramebufferSizeCallback(mWindow, OnWindowResize);
	glfwSetCursorPosCallback(mWindow, OnMouseMoved);
	glfwSetScrollCallback(mWindow, OnMouseScroll);
	glfwSetKeyCallback(mWindow, OnKeyAction);
	glfwSetMouseButtonCallback(mWindow, OnMouseAction);
	glfwMakeContextCurrent(mWindow);
}

windows_window::~windows_window() {
	glfwTerminate();
}

window_process_result windows_window::ProcessExternalEvents() {
	window_process_result Result;
	Result.mInput.mLastFrame = mFrameInput;
	proc_data Data{Result, mFrameInput, *this};
	glfwSetWindowUserPointer(mWindow, &Data);
	glfwPollEvents();
	glfwSetWindowUserPointer(mWindow, nullptr);
	Result.mInput.mThisFrame = mFrameInput;
	Result.mState = mState;
	return Result;
}

dyn_array<app_message> windows_window::HandleMessages(const dyn_array<app_message>& Messages) {
	dyn_array<app_message> OutMessages;
	for (const auto& Message : Messages) {
		switch (Message.mType) {
			case app_message_type::window_close:
				glfwSetWindowShouldClose(mWindow, true);
				break;
			case app_message_type::window_vsync:
				mState.mVSync = Message.mWindowVsync.mVSync;
				glfwSwapInterval(mState.mVSync ? 1 : 0);
				break;
			case app_message_type::window_cursor:
				mState.mCursorEnabled = Message.mWindowCursor.mCursor;
				glfwSetInputMode(
					mWindow, GLFW_CURSOR, mState.mCursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
				break;
			default:
				OutMessages.Add(Message);
				break;
		}
	}
	return OutMessages;
}

bool windows_window::ShouldClose() const {
	return glfwWindowShouldClose(mWindow);
}

void windows_window::SwapBuffers() {
	glfwSwapBuffers(mWindow);
}

#endif