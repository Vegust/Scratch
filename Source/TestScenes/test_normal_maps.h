#pragma once

#include "test_scene.h"

class test_normal_maps : public test_scene {
public:
	test_normal_maps();

	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	virtual void OnScreenSizeChanged(int NewWidth, int NewHeight) override;
	virtual void OnUpdate(float DeltaTime) override;

	std::shared_ptr<camera> mCamera{nullptr};

	shader mLightShader{};

	phong_material mCubeMaterial;
	dyn_array<glm::mat4> mStaticCubes;

	float mAccTime = 0.f;
	float mUpdateSpeed = 1.f;

	framebuffer mSceneFramebuffer;
};
