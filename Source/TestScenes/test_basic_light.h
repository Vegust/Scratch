//
// Created by Vegust on 25.06.2023.
//
#pragma once

#include "core_types.h"
SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/glm.hpp"
SCRATCH_DISABLE_WARNINGS_END()

#include "Rendering/index_buffer.h"
#include "Rendering/shader.h"
#include "Rendering/vertex_array.h"
#include "Rendering/vertex_buffer.h"
#include "SceneObjects/phong_material.h"
#include "SceneObjects/light.h"
#include "test_scene.h"

#include <array>

class test_basic_light : public test_scene
{
public:
	test_basic_light();
	virtual ~test_basic_light() override;
	
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	
private:
	std::unique_ptr<shader> Shader{nullptr};
	std::unique_ptr<shader> LightShader{nullptr};
	std::shared_ptr<camera> Camera{nullptr};
	
	glm::vec3 CubePosition{0.7f,0.f,-1.5f};
	phong_material CubeMaterial{};
	
	std::vector<light> Lights{};
	
	float CurrentRotation = 0.f;
	bool bRotating = true;
};
