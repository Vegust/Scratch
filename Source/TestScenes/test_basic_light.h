//
// Created by Vegust on 25.06.2023.
//
#pragma once

#include "core_types.h"
SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/glm.hpp"
SCRATCH_DISABLE_WARNINGS_END()

#include <array>

#include "test_scene.h"
#include "Rendering/vertex_array.h"
#include "Rendering/vertex_buffer.h"
#include "Rendering/shader.h"
#include "Rendering/index_buffer.h"

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
	glm::vec3 LightPosition{-0.7f,0.f,0.f};
	
	float CurrentRotation = 0.f;
	
	float AmbientStrength = 0.1f;
	float SpecularStrength = 0.5f;
	int32 SpecularPower = 32;
	std::array<float,4> LightColor{1.0f, 1.0f, 1.0f, 1.f};
	std::array<float,4> CubeColor{1.0f, 0.5f, 0.31f, 1.f};
	bool bRotating = true;
};
