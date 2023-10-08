#pragma once

#include "core_types.h"
#include "Rendering/rendering_types.h"

// used as user data pointer in platform callbacks
class application;
struct GLFWwindow;

class window_windows {
public:
	GLFWwindow* mWindow{nullptr};
	
	~window_windows();
	
	void Init(application* App, u32 WindowWidth, u32 WindowHeight);
	
	application* GetApplication();
	application* GetApplication(GLFWwindow* Window);
	
	bool ShouldClose();
	
	void ProcessEvents();
	void SetContextCurrent();
	void SetVSync(bool Enabled);
	void SwapBuffers();
	void* GetApiLoadingFunction(rendering_api Api);
};