#include "windows_window.h"

#ifdef WIN32

#include "Rendering/rendering_types.h"
#include "glfw_keykodes_table.h"
#include <GLFW/glfw3.h>

struct proc_data {
	window_process_result& Result;
	frame_input_state& FrameInput;
	windows_window& Window;
};

static proc_data* GetData(struct GLFWwindow* Window) {
	return (proc_data*) (glfwGetWindowUserPointer(Window));
}

static void OnWindowResize(GLFWwindow* Window, int NewWidth, int NewHeight) {
	proc_data* Data = GetData(Window);
	Data->Window.State.Height = NewHeight;
	Data->Window.State.Width = NewWidth;
	const auto Index = Data->Result.Messages.FindFirstByPredicate(
		[](const auto& Message) { return Message.Type == app_message_type::render_resize; });
	if (Index != InvalidIndex) {
		Data->Result.Messages[Index].RenderResize.NewWidth = NewWidth;
		Data->Result.Messages[Index].RenderResize.NewHeight = NewHeight;
	} else {
		app_message ResizeMessage;
		ResizeMessage.Type = app_message_type::render_resize;
		ResizeMessage.RenderResize.NewWidth = NewWidth;
		ResizeMessage.RenderResize.NewHeight = NewHeight;
		Data->Result.Messages.Add(ResizeMessage);
	}
}

static void OnMouseMoved(GLFWwindow* Window, double XPos, double YPos) {
	proc_data* Data = GetData(Window);
	auto& MouseState = Data->FrameInput.MouseState;
	MouseState.Pos = {XPos, YPos};
}

static void OnMouseScroll(GLFWwindow* Window, double XDelta, double YDelta) {
	proc_data* Data = GetData(Window);
	auto& MouseState = Data->FrameInput.MouseState;
	MouseState.Scroll = {XDelta, YDelta};
}

static void OnKeyChange(GLFWwindow* Window, input_key Key, s32 Action) {
	key_state KeyState;
	if (Action == GLFW_PRESS || Action == GLFW_REPEAT) {
		KeyState = key_state::pressed;
	} else if (Action == GLFW_RELEASE) {
		KeyState = key_state::released;
	}
	proc_data* Data = GetData(Window);
	Data->FrameInput.KeyStates[static_cast<u32>(Key)] = KeyState;
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
	State.Height = WindowHeight;
	State.Width = WindowWidth;
	glfwInit();
	Window = glfwCreateWindow((s32) WindowWidth, (s32) WindowHeight, "Scratch", nullptr, nullptr);
	CHECK(Window)
	glfwSwapInterval(State.VSync ? 1 : 0);
	glfwSetFramebufferSizeCallback(Window, OnWindowResize);
	glfwSetCursorPosCallback(Window, OnMouseMoved);
	glfwSetScrollCallback(Window, OnMouseScroll);
	glfwSetKeyCallback(Window, OnKeyAction);
	glfwSetMouseButtonCallback(Window, OnMouseAction);
	glfwMakeContextCurrent(Window);
}

windows_window::~windows_window() {
	glfwTerminate();
}

window_process_result windows_window::ProcessExternalEvents() {
	window_process_result Result;
	Result.Input.LastFrame = FrameInput;
	proc_data Data{Result, FrameInput, *this};
	glfwSetWindowUserPointer(Window, &Data);
	glfwPollEvents();
	glfwSetWindowUserPointer(Window, nullptr);
	Result.Input.ThisFrame = FrameInput;
	Result.State = State;
	return Result;
}

dyn_array<app_message> windows_window::HandleMessages(const dyn_array<app_message>& Messages) {
	dyn_array<app_message> OutMessages;
	for (const auto& Message : Messages) {
		switch (Message.Type) {
			case app_message_type::window_close:
				glfwSetWindowShouldClose(Window, true);
				break;
			case app_message_type::window_vsync:
				State.VSync = Message.WindowVsync.VSync;
				glfwSwapInterval(State.VSync ? 1 : 0);
				break;
			case app_message_type::window_cursor:
				State.CursorEnabled = Message.WindowCursor.Cursor;
				glfwSetInputMode(
					Window, GLFW_CURSOR, State.CursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
				break;
			default:
				OutMessages.Add(Message);
				break;
		}
	}
	return OutMessages;
}

bool windows_window::ShouldClose() const {
	return glfwWindowShouldClose(Window);
}

void windows_window::SwapBuffers() {
	glfwSwapBuffers(Window);
}

#endif