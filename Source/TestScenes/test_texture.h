//
// Created by Vegust on 22.06.2023.
//

#pragma once

#include "core_types.h"
SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/glm.hpp"
SCRATCH_DISABLE_WARNINGS_END()

#include "Rendering/element_buffer.h"
#include "Rendering/shader.h"
#include "Rendering/texture.h"
#include "Rendering/vertex_array.h"
#include "Rendering/vertex_buffer.h"
#include "test_scene.h"

class test_texture : public test_scene
{
public:
	test_texture();
	virtual ~test_texture() override;
	
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	
private:
	vertex_buffer VertexBuffer{};
	vertex_array VertexArray{};
	element_buffer IndexBuffer{};
	texture Texture{};
	shader Shader{};
	
	glm::vec3 Pic1Trans{0.5f,0.f,0.f};
	glm::vec3 Pic2Trans{-0.5f,0.f,0.f};
};
