#pragma once

#include "Platform/window.h"
#include "Input/input_state.h"
#include "Rendering/rendering_context.h"
#include "Rendering/rendering_types.h"
#include "Rendering/OldRender/renderer.h"
#include "test_shadowmaps.h"

class application {
protected:
	// TODO refactor -----------------------------------
	rendering_api mRenderingApi{rendering_api::opengl};
	rendering_context mRenderingContext{};
	renderer mOldRenderer{};
	// ------------------------------------------------

	// ------------------------------------------------
	window mWindow{};
	input_state mInputState{};
	
	// --------- ???? -------------
	float mLastFrameTime{0};
	float mDeltaTime{0};

public:
	// TODO remove ------------------------------------
	test_shadowmaps TestMap{};

	window& GetWindow() {
		return mWindow;
	}

	input_state& GetInputState() {
		return mInputState;
	}

	renderer& GetRenderer() {
		return mOldRenderer;
	}

	application();
	void Run();
};