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
	float LastFrameTime{0};

public:
	// TODO remove ------------------------------------
	test_shadowmaps TestMap{};

	const window& GetWindow() const {
		return mWindow;
	}

	const input_state& GetInputState() const {
		return mInputState;
	}

	renderer& GetRenderer() {
		return mOldRenderer;
	}

	application();
	void Run();
};