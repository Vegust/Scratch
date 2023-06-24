//
// Created by Vegust on 24.06.2023.
//
#pragma once

#include "core_types.h"
SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/glm.hpp"
SCRATCH_DISABLE_WARNINGS_END()

#include "test_scene.h"
#include "Rendering/vertex_array.h"
#include "Rendering/vertex_buffer.h"
#include "Rendering/shader.h"
#include "Rendering/index_buffer.h"

class test_3d_texture : public test_scene
{
public:
	test_3d_texture();
	virtual ~test_3d_texture() override;
	
	virtual void OnUpdate() override;
	virtual void OnRender() override;
	virtual void OnIMGuiRender() override;
	
private:
	std::unique_ptr<vertex_buffer> VertexBuffer{nullptr};
	std::unique_ptr<vertex_array> VertexArray{nullptr};
	std::unique_ptr<index_buffer> IndexBuffer{nullptr};
	std::unique_ptr<shader> Shader{nullptr};
	
	glm::vec3 Pic1Trans{0.f,0.f,0.f};
	glm::vec3 Pic1Rot{0.f,0.f,0.f};
};
