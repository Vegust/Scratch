#pragma once

#include "Rendering/rendering_types.h"
#include "Rendering/OldRender/old_rebderer.h"
#include "Rendering/Backend/dynamic_rhi.h"

class renderer {
public:
	rendering_api mRenderingApi{rendering_api::opengl};
	old_rebderer mOldRenderer{};
	dynamic_rhi* mRHI{nullptr};

	void Init(u32 WindowWidth, u32 WindowHeight);
	void RenderView();
};