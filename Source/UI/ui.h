#pragma once

#include "core_types.h"

enum class rendering_api : u8;
class application;

// ImGUI works through static calls to global state, so rigth now this class is not very useful
struct ui {
	application* mParentApp{nullptr};

	[[nodiscard]] bool ConsumesMouseInput() const;
	[[nodiscard]] bool ConsumesKeyboardInput() const;

	void Init(application* App, rendering_api Api);
	void StartNewFrame();
	void StartDebugWindow(float DeltaTime);
	void EndDebugWindow();
	void Render();
};