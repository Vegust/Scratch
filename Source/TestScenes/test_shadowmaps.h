//
// Created by Vegust on 01.07.2023.
//

#pragma once

#include "test_scene.h"

class test_shadowmaps : public test_scene
{
public:
	test_shadowmaps();
	
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	virtual void OnScreenSizeChanged(int NewWidth, int NewHeight) override;
	virtual void OnUpdate(float DeltaTime) override;
	
	phong_material CubeMaterial;
	std::shared_ptr<camera> Camera{nullptr};
	
	std::vector<glm::mat4> StaticCubes;
	std::vector<glm::mat4> DynamicCubes;
	
	framebuffer DirectionalShadowmap;
	shader DirectionalShadowmapShader;

	int32 CubeEditIndex = 0;
	int32 LightEditIndex = 0;
	framebuffer SceneFramebuffer;
	
	bool bDrawShadowmap = false;
	
	float AccTime = 0.f;
	float UpdateSpeed = 1.f;
	
	void SetupCubeTransforms();
	void UpdateDynamicCubeTransforms(float DeltaTime);
};
