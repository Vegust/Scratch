#pragma once

#include "core_types.h"
#include "glm/glm.hpp"

#include "Rendering/element_buffer.h"
#include "Rendering/shader.h"
#include "Rendering/vertex_array.h"
#include "Rendering/vertex_buffer.h"
#include "SceneObjects/light.h"
#include "SceneObjects/phong_material.h"
#include "test_scene.h"

class test_basic_light : public test_scene {
public:
	test_basic_light();

	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;

private:
	shader mShader{};
	shader mLightShader{};

	std::shared_ptr<camera> mCamera{nullptr};

	glm::vec3 mCubePosition{0.7f, 0.f, -1.5f};
	phong_material mCubeMaterial{};

	dyn_array<light> mLights{};

	float mCurrentRotation = 0.f;
	bool mRotating = true;
};
