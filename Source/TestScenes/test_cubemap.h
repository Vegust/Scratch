#pragma once

#include "test_scene.h"
#include "Rendering/OldRender/cubemap.h"

class test_cubemap : public test_scene {
public:
	test_cubemap();

	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;

	enum mat_type { mat_default = 0, mat_mirror = 1, mat_glass = 2 };

	mat_type mMatType = mat_default;
	float mRefractiveIndex = 1.52;
	shader mMirrorShader{};
	shader mGlassShader{};

	std::shared_ptr<camera> mCamera{nullptr};
	glm::vec3 mCubePosition{0.0f, 0.f, -1.5f};
	phong_material mCubeMaterial{};
	cubemap mSkybox;
};
