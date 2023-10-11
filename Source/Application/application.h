#pragma once

#include "Platform/window.h"
#include "Rendering/renderer.h"
#include "Game/game.h"

class application {
public:
	window mWindow;
	renderer mRenderer;
	game mGame;
	float mTime;

	application(u32 WindowWidth, u32 WindowHeight);
	bool RunOneFrame();
};