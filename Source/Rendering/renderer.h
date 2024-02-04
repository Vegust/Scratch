#pragma once

#include "Core/Ownership//owned.h"
#include "Rendering/rendering_types.h"
#include "Rendering/OldRender/old_rebderer.h"
#include "Application/app_message.h"
#include "view.h"
#include "Rendering/Backend/dynamic_rhi.h"

struct ui_data;

class renderer {
public:
	old_rebderer mOldRenderer{};
	render_state mState;
	owned<rhi> mRHI{nullptr};

	// from test_shadowmaps, toto refactor
	phong_material mCubeMaterial;
	dyn_array<glm::mat4> mStaticCubes;
	dyn_array<glm::mat4> mDynamicCubes;
	framebuffer mPointShadowmap;
	shader mPointShadowmapShader;
	framebuffer mDirectionalShadowmap;
	shader mDirectionalShadowmapShader;
	framebuffer mSceneFramebuffer;
	cubemap mSkybox;
	bool mDrawShadowmap = false;
	float mAccTime = 0.f;
	float mUpdateSpeed = 1.f;
	s32 mShadowmapResolution = 2048;

	renderer(u32 WindowWidth, u32 WindowHeight);
	~renderer();

	render_state HandleMessages(const dyn_array<app_message>& Messages);
	void RenderViews(const dyn_array<view>& Views);
	void RenderUI(const ui_data& UIData);
};