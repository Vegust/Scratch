#pragma once

#include "Platform/window.h"
#include "Rendering/renderer.h"
#include "Game/game.h"

struct application_settings {
public:
	u32 WindowWidth{1000};
	u32 WindowHeight{800};
};

class application {
public:
	application_settings Settings;
	
	window Window;
	renderer Renderer;
	game Game;
	float Time;

	explicit application(const application_settings& InSettings);
	bool RunOneFrame();
};