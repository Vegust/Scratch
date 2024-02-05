#include "windows_platform.h"

#ifdef WIN32

#include <windows.h>
#include "Rendering/rendering_types.h"
#include "GLFW/glfw3.h"

float windows_platform::GetTime() {
	return static_cast<float>(glfwGetTime());
}

void windows_platform::InitUi(ui& UI, window& Window, rendering_api Api) {
	//	switch (Api) {
	//		case rendering_api::opengl:
	//			ImGui_ImplGlfw_InitForOpenGL(Window.GetGLFWWindow(), true);
	//			break;
	//	}
}

void windows_platform::UpdateUi(ui& UI, window& Window) {
	// ImGui_ImplGlfw_NewFrame();
}

void* windows_platform::GetApiLoadingFunction(rendering_api Api) {
	switch (Api) {
		case rendering_api::opengl:
			return (void*) (glfwGetProcAddress);
	}
	return nullptr;
}

timestamp windows_platform::GetUTC() {
	FILETIME Time;
	GetSystemTimePreciseAsFileTime(&Time);
	// January 1, 1601 (UTC).
	s64 SystemTicks = (((s64) Time.dwHighDateTime) << 32) + (s64) Time.dwLowDateTime;
	constexpr s64 InitialTicks = timestamp::DateTimeToTicks(1601, 1, 1);
	return timestamp{InitialTicks + SystemTicks};
}

timezone windows_platform::GetTimezone() {
	TIME_ZONE_INFORMATION TimezoneInfo;
	GetTimeZoneInformation(&TimezoneInfo);
	return timezone{TimezoneInfo.Bias * timestamp::TicksPerMinute};
}

#endif