#include "game.h"

#include "Asset/asset_storage.h"

static ui_data UpdateUi() {
	return {};
}

game::game() {
	mWorlds.Emplace();
}

game_update_result game::Step(
	float Time,
	float DeltaTime,
	const input& Input,
	const window_state& WindowState,
	const render_state& RenderState) {
	game_update_result Result;
	Result.mUIData = UpdateUi();
	return Result;
}

