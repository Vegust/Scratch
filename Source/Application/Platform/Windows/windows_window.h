#pragma once

#ifdef WIN32

#include "basic.h"
#include "Application/Platform/window_types.h"
#include "Application/Input/input.h"
#include "Application/app_message.h"

struct GLFWwindow;

struct window_process_result {
	input Input;
	dyn_array<app_message> Messages;
	window_state State;
};

class windows_window {
public:
	GLFWwindow* Window{nullptr};

	frame_input_state FrameInput;
	window_state State{};

	windows_window(u32 WindowWidth, u32 WindowHeight);
	~windows_window();

	window_process_result ProcessExternalEvents();
	dyn_array<app_message> HandleMessages(const dyn_array<app_message>& Messages);

	[[nodiscard]] bool ShouldClose() const;
	void SwapBuffers();
};

#endif