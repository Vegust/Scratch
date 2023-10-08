#pragma once

#include "SceneObjects/phong_material.h"
#include "SceneObjects/camera.h"
#include "Rendering/OldRender/framebuffer.h"
#include "Rendering/OldRender/shader.h"
#include "Rendering/OldRender/cubemap.h"

class renderer;

class test_shadowmaps {
public:
	void Init(renderer& Renderer);
	void OnRender(renderer& Renderer);
	void OnIMGuiRender(renderer& Renderer);
	void OnScreenSizeChanged(int NewWidth, int NewHeight);
	void OnUpdate(float DeltaTime);

	phong_material mCubeMaterial;
	std::shared_ptr<camera> mCamera{nullptr};

	dyn_array<glm::mat4> mStaticCubes;
	dyn_array<glm::mat4> mDynamicCubes;

	framebuffer mPointShadowmap;
	shader mPointShadowmapShader;

	framebuffer mDirectionalShadowmap;
	shader mDirectionalShadowmapShader;

	s32 mCubeEditIndex = 0;
	s32 mLightEditIndex = 0;
	framebuffer mSceneFramebuffer;
	
	cubemap mSkybox;

	bool mDrawShadowmap = false;

	float mAccTime = 0.f;
	float mUpdateSpeed = 1.f;

	s32 mShadowmapResolution = 2048;

	void SetupCubeTransforms();
	void UpdateDynamicCubeTransforms(float DeltaTime);
};
