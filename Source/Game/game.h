#pragma once

#include "Application/app_message.h"
#include "Rendering/view.h"
#include "UI/ui_data.h"
#include "world.h"

struct input;
struct window_state;
struct render_state;

struct game_update_result {
	dyn_array<view> Views;
	ui_data UIData;
	dyn_array<app_message> Messages;
};

class game {
public:
	dyn_array<world> Worlds{};

	game();
	game_update_result Step(
		float Time,
		float DeltaTime,
		const input& Input,
		const window_state& WindowState,
		const render_state& RenderState);
};