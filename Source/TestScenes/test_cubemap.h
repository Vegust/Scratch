//
// Created by Vegust on 29.06.2023.
//

#pragma once

#include "Rendering/cubemap.h"
#include "test_scene.h"

class test_cubemap : public test_scene
{
public:
	test_cubemap();
	
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	
	std::shared_ptr<camera> Camera{nullptr};
	glm::vec3 CubePosition{0.0f,0.f,-1.5f};
	phong_material CubeMaterial{};
	cubemap Skybox;
};
