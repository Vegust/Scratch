//
// Created by Vegust on 22.06.2023.
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
#include "Rendering/texture.h"
#include "Rendering/index_buffer.h"

class test_texture : public test_scene
{
public:
	test_texture();
	virtual ~test_texture() override;
	
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	
private:
	std::unique_ptr<vertex_buffer> VertexBuffer{nullptr};
	std::unique_ptr<vertex_array> VertexArray{nullptr};
	std::unique_ptr<index_buffer> IndexBuffer{nullptr};
	std::unique_ptr<texture> Texture{nullptr};
	std::unique_ptr<shader> Shader{nullptr};
	
	glm::vec3 Pic1Trans{0.5f,0.f,0.f};
	glm::vec3 Pic2Trans{-0.5f,0.f,0.f};
};
