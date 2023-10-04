#pragma once

#include "core_types.h"
#include "glm/glm.hpp"

#include "Rendering/OldRender/element_buffer.h"
#include "Rendering/OldRender/shader.h"
#include "Rendering/OldRender/texture.h"
#include "Rendering/OldRender/vertex_array.h"
#include "Rendering/OldRender/vertex_buffer.h"
#include "test_scene.h"

class test_texture : public test_scene {
public:
	test_texture();
	virtual ~test_texture() override;

	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;

private:
	vertex_buffer mVertexBuffer{};
	vertex_array mVertexArray{};
	element_buffer mIndexBuffer{};
	texture mTexture{};
	shader mShader{};

	glm::vec3 mPic1Trans{0.5f, 0.f, 0.f};
	glm::vec3 mPic2Trans{-0.5f, 0.f, 0.f};
};
