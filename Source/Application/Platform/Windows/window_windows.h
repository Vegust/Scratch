#pragma once

#ifdef WIN32

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
	void ProcessEvents(renderer& Renderer, float DeltaTime);
	void SetContextCurrent();
	void SwapBuffers();
	void CloseWindow();

	void SetCursorEnabled(bool Enabled);
	[[nodiscard]] bool GetCursorEnabled() const;
	void SetVSync(bool Enabled);
	[[nodiscard]] bool GetVSync() const;

	void* GetApiLoadingFunction(rendering_api Api);
};

#endif