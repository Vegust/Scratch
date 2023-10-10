#pragma once

#include "core_types.h"

enum class rendering_api : u8;
class application;

struct ui {
	application* mParentApp{nullptr};

	void Init(application* App, rendering_api Api);
	void OnNewFrame();
	void StartDebugWindow(float DeltaTime);
	void EndDebugWindow();
	void Render();
};