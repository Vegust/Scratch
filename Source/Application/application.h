#pragma once

#include "Platform/window.h"
#include "Input/input_state.h"
#include "UI/ui.h"

#include "Rendering/rendering_types.h"
#include "Rendering/OldRender/renderer.h"
#include "Rendering/RHI/dynamic_rhi.h"

#include "test_shadowmaps.h"

class application {
public:
	// TODO refactor -----------------------------------
	rendering_api mRenderingApi{rendering_api::opengl};
	renderer mOldRenderer{};
	std::unique_ptr<dynamic_rhi> mRHI{nullptr};
	// ------------------------------------------------

	// TODO remove ------------------------------------
	test_shadowmaps mTestMap{};

	ui mUI{};
	window mWindow{};
	input_state mInputState{};
	float mLastFrameTime{0};
	float mDeltaTime{0};

	application();
	void Run();
};