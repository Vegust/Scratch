#include "platform_windows.h"

#ifdef WIN32

#include "Rendering/rendering_types.h"
#include "Application/Platform/window.h"
#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"

float platform_windows::GetTime() {
	return static_cast<float>(glfwGetTime());
}

void platform_windows::InitUi(ui& UI, window& Window, rendering_api Api) {
//	switch (Api) {
//		case rendering_api::opengl:
//			ImGui_ImplGlfw_InitForOpenGL(Window.GetGLFWWindow(), true);
//			break;
//	}
}

void platform_windows::UpdateUi(ui& UI, window& Window) {
//	ImGui_ImplGlfw_NewFrame();
}

void* platform_windows::GetApiLoadingFunction(rendering_api Api) {
	switch (Api) {
		case rendering_api::opengl:
			return (void*) (glfwGetProcAddress);
	}
	return nullptr;
}

#endif