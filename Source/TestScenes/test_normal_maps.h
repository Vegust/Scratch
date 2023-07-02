//
// Created by Vegust on 02.07.2023.
//

#pragma once

#include "test_scene.h"

class test_normal_maps : public test_scene
{
public:
	test_normal_maps();
	
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	virtual void OnScreenSizeChanged(int NewWidth, int NewHeight) override;
	virtual void OnUpdate(float DeltaTime) override;
	
	std::shared_ptr<camera> Camera{nullptr};
	
	shader LightShader{};

	phong_material CubeMaterial;
	std::vector<glm::mat4> StaticCubes;
	
	float AccTime = 0.f;
	float UpdateSpeed = 1.f;
	
	framebuffer SceneFramebuffer;
};
