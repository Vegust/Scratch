#pragma once

#ifdef WIN32

#include "core_types.h"

class windows_window;
using window = windows_window;
struct ui;

enum class rendering_api : u8;

struct windows_platform {
	static float GetTime();

	static void* GetApiLoadingFunction(rendering_api Api);

	// TODO this is ugly:
	static void InitUi(ui& UI, window& Window, rendering_api Api);
	static void UpdateUi(ui& UI, window& Window);
};

#endif