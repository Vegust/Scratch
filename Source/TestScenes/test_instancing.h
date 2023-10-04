#pragma once

#include "Rendering/OldRender/cubemap.h"
#include "SceneObjects/model.h"
#include "test_scene.h"

class test_instancing : public test_scene {
public:
	test_instancing();

	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;

	shader mInstancedPhongShader{};
	shader mSunShader{};
	model mModel;
	cubemap mSkybox;
	std::shared_ptr<camera> mCamera{nullptr};

	bool mInstancing = false;
	s32 mAsteroidCount = 100;
	float mInnerRadius = 6.f;
	float mOuterRadius = 6.5f;
	float mVerticalSpread = 0.5f;

	vertex_buffer mInstancesBuffer{};
	dyn_array<glm::mat4> mAsteroidTransforms{};

	void RecalculateAsteroidTransforms();
};
