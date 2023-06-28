//
// Created by Vegust on 24.06.2023.
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

class test_3d_texture : public test_scene
{
public:
	test_3d_texture();
	virtual ~test_3d_texture() override;
	
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	
private:
	vertex_buffer VertexBuffer{};
	vertex_array VertexArray{};
	element_buffer IndexBuffer{};
	texture Texture{};
	shader Shader{};
	
	glm::vec3 Pic1Trans{0.f,0.f,0.f};
	glm::vec3 Pic1Rot{0.f,0.f,0.f};
};
