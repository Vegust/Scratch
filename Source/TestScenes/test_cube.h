#pragma once

#include "core_types.h"
#include "glm/glm.hpp"

#include "Rendering/element_buffer.h"
#include "Rendering/shader.h"
#include "Rendering/texture.h"
#include "Rendering/vertex_array.h"
#include "Rendering/vertex_buffer.h"
#include "test_scene.h"

class test_cube : public test_scene {
public:
	test_cube();
	virtual ~test_cube() override;

	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;

private:
	shader mShader{};
	texture mTexture{};

	glm::vec3 mPic1Trans{0.f, 0.f, 0.f};
	glm::vec3 mPic1Rot{0.f, 0.f, 0.f};

	float mCurrentRotation = 0.f;
	s32 mNumCubes = 5;
	u32 mSeed{0};
};
