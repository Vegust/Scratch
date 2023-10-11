#pragma once

#include "test_shadowmaps.h"
#include "Application/app_message.h"
#include "Rendering/view.h"
#include "UI/ui_data.h"

struct input;
struct window_state;
struct render_state;

struct game_update_result {
	dyn_array<view> mViews;
	ui_data mUIData;
	dyn_array<app_message> mMessages;
};

class game {
public:
	// TODO refactor
	test_shadowmaps mTestMap{};

	game();

	game_update_result Update(
		float Time,
		float DeltaTime,
		const input& Input,
		const window_state& WindowState,
		const render_state& RenderState);
};