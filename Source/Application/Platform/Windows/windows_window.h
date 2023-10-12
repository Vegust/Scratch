#pragma once

#ifdef WIN32

#include "core_types.h"
#include "Application/Platform/window_types.h"
#include "Application/Input/input.h"
#include "Application/app_message.h"

struct GLFWwindow;

struct window_process_result {
	input mInput;
	dyn_array<app_message> mMessages;
	window_state mState;
};

class windows_window {
public:
	GLFWwindow* mWindow{nullptr};

	frame_input_state mFrameInput;
	window_state mState{};

	windows_window(u32 WindowWidth, u32 WindowHeight);
	~windows_window();

	window_process_result ProcessExternalEvents();
	dyn_array<app_message> HandleMessages(const dyn_array<app_message>& Messages);

	[[nodiscard]] bool ShouldClose() const;
	void SwapBuffers();
};

#endif