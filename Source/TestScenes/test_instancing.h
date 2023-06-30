//
// Created by Vegust on 30.06.2023.
//

#pragma once

#include "Rendering/cubemap.h"
#include "SceneObjects/model.h"
#include "test_scene.h"

class test_instancing : public test_scene
{
public:
	test_instancing();
	
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	
	shader InstancedPhongShader{};
	shader SunShader{};
	model Model;
	cubemap Skybox;
	std::shared_ptr<camera> Camera{nullptr};
	
	bool bInstancing = false;
	int32 AsteroidCount = 100;
	float InnerRadius = 6.f;
	float OuterRadius = 6.5f;
	float VerticalSpread = 0.5f;
	
	vertex_buffer InstancesBuffer{};
	
	std::vector<glm::mat4> AsteroidTransforms{};
	
	void RecalculateAsteroidTransforms();
};

