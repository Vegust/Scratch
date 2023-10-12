#pragma once

#include "core_types.h"

struct ui_data {
	[[nodiscard]] bool ConsumesMouseInput() const; // mouse cursor enabled and hovered over ui
	[[nodiscard]] bool ConsumesKeyboardInput() const; // ui focused

	void StartDebugWindow(float DeltaTime);
	void EndDebugWindow();
};