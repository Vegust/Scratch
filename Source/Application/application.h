#pragma once

#include "Platform/window.h"
#include "Rendering/renderer.h"
#include "UI/ui.h"
#include "Game/game.h"
#include "Input/input_state.h"

class application {
public:
	window mWindow{};
	renderer mRenderer{};
	ui mUI{};
	game mGame{};
	input_state mInputState{};

	float mCurrentTime{0};

	application();
	void Run();
};