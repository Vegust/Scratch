#pragma once

#ifdef WIN32

#include "core_types.h"

class application;
class old_rebderer;
struct GLFWwindow;
struct input_state;
enum class rendering_api : u8;

class window_windows {
private:
	GLFWwindow* mWindow{nullptr};
	bool mVSync{true};
	bool mCursorEnabled{true};
	u32 mWindowHeight{0};
	u32 mWindowWidth{0};
public:
	~window_windows();

	void Init(application* App, u32 WindowWidth, u32 WindowHeight);

	bool ShouldClose();
	void CloseWindow();

	void ProcessEvents();

	void SetContextCurrent();
	void SwapBuffers();

	void SetCursorEnabled(bool Enabled);
	void SetVSync(bool Enabled);
	void SetWindowHeight(u32 Height);
	void SetWindowWidth(u32 Width);
	[[nodiscard]] bool GetCursorEnabled() const;
	[[nodiscard]] bool GetVSync() const;
	[[nodiscard]] u32 GetWindowHeight() const;
	[[nodiscard]] u32 GetWindowWidth() const;

	// Windows Specific
	GLFWwindow* GetGLFWWindow();
};

#endif