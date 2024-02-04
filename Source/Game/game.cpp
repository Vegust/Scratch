#include "game.h"

#include "Asset/asset_storage.h"

static ui_data UpdateUi() {
	return {};
}

game::game() {
	Worlds.Emplace();
}

game_update_result game::Step(
	float Time,
	float DeltaTime,
	const input& Input,
	const window_state& WindowState,
	const render_state& RenderState) {
	game_update_result Result;
	Result.UIData = UpdateUi();
	return Result;
}

