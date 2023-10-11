#pragma once

#include "core_types.h"

struct ui_data {
	[[nodiscard]] bool ConsumesMouseInput() const;
	[[nodiscard]] bool ConsumesKeyboardInput() const;

	void StartDebugWindow(float DeltaTime);
	void EndDebugWindow();
};