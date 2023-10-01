#pragma once

#include "test_scene.h"

class test_shadowmaps : public test_scene {
public:
	test_shadowmaps();

	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	virtual void OnScreenSizeChanged(int NewWidth, int NewHeight) override;
	virtual void OnUpdate(float DeltaTime) override;

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

	bool mDrawShadowmap = false;

	float mAccTime = 0.f;
	float mUpdateSpeed = 1.f;

	s32 mShadowmapResolution = 2048;

	void SetupCubeTransforms();
	void UpdateDynamicCubeTransforms(float DeltaTime);
};
