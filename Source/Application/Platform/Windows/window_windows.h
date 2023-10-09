#pragma once

#include "core_types.h"

class application;
class renderer;
struct GLFWwindow;
struct input_state;
enum class rendering_api : u8;

class window_windows {
public:
	GLFWwindow* mWindow{nullptr};
	bool mVSync{true};
	bool mCursorEnabled{true};

	~window_windows();

	void Init(application* App, u32 WindowWidth, u32 WindowHeight);

	bool ShouldClose();
	void ProcessEvents(input_state& InputState, renderer& Renderer, float DeltaTime);
	void SetContextCurrent();
	void SwapBuffers();
	void SetCursorEnabled(bool Enabled);

	[[nodiscard]] bool GetCursorEnabled() const {
		return mCursorEnabled;
	}

	void SetVSync(bool Enabled);

	[[nodiscard]] bool GetVSync() const {
		return mVSync;
	}

	void* GetApiLoadingFunction(rendering_api Api);
};