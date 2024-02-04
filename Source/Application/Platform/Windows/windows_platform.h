#pragma once

#ifdef WIN32

#include "basic.h"

class windows_window;
using window = windows_window;
struct ui;
enum class rendering_api : u8;

namespace windows_platform {
float GetTime();

// 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
s64 GetUTC();

void* GetApiLoadingFunction(rendering_api Api);

// TODO this is ugly:
void InitUi(ui& UI, window& Window, rendering_api Api);
void UpdateUi(ui& UI, window& Window);
};	  // namespace windows_platform

#endif