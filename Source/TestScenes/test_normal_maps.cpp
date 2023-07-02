//
// Created by Vegust on 02.07.2023.
//

#include "test_normal_maps.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

REGISTER_TEST_SCENE(test_normal_maps, "10 Normals")

test_normal_maps::test_normal_maps()
{
	auto& Light = renderer::Get().SceneLights.emplace_back();
	Light.Position = {0.f, 0.f, 2.f};
	Light.Specular = {0.5f,0.5f,0.5f};
	
	LightShader.Compile("Resources/Shaders/BasicLight.shader");

	CubeMaterial.InitTextures(
		"Resources/Textures/Bricks/brickwall.jpg", 0, "Resources/Textures/DefaultSpecular.jpg", 1, "",2,"Resources/Textures/Bricks/brickwall_normal.jpg", 3);
	CubeMaterial.Shininess = 8.f;
	SceneFramebuffer.Reload();

	Camera = std::make_shared<camera>();
	Camera->Position = glm::vec3{0.f, 0.5f, 2.f};
	renderer::Get().CustomCamera = Camera;

	StaticCubes.push_back(glm::mat4(1.f));
}

void test_normal_maps::OnUpdate(float DeltaTime)
{
	AccTime += DeltaTime * UpdateSpeed;

	auto& Light = renderer::Get().SceneLights[0];
	const float Rotation = glm::pi<float>() * AccTime;
	Light.Position = glm::translate(
						 glm::rotate(glm::mat4(1.f), Rotation, glm::vec3(0.5, 0.5, 0.1)),
						 glm::vec3(0.f, 0.f, 2.f)) *
					 glm::vec4(0.f, 0.f, 0.f, 1.f);
}

void test_normal_maps::OnRender(renderer& Renderer)
{
	glClearColor(0.f, 1.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	SceneFramebuffer.Bind();
	glClearColor(0.03f, 0.03f, 0.03f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	Renderer.DrawCubes(CubeMaterial, StaticCubes);
	
	// Light
	const glm::mat4 View = Camera->GetViewTransform();
	std::vector<glm::mat4> LightTransforms;
	const float Rotation = glm::pi<float>() * AccTime;
	glm::mat4 LightTransform = glm::scale(
		glm::translate(glm::mat4{1.0f}, renderer::Get().SceneLights[0].Position),
		glm::vec3(0.1f, 0.1f, 0.1f));
	LightTransforms.push_back(LightTransform);
	LightShader.Bind();
	LightShader.SetUniform("u_Light", renderer::Get().SceneLights[0], View);
	Renderer.DrawNormalCubes(LightShader, LightTransforms);
	
	framebuffer::SetDefault();

	Renderer.DrawFrameBuffer(SceneFramebuffer);
}

void test_normal_maps::OnIMGuiRender()
{
	ImGui::SliderFloat("Slomo", &UpdateSpeed, 0.f, 2.f);
	if (ImGui::CollapsingHeader("Light"))
	{
		renderer::Get().SceneLights[0].UIControlPanel("");
	}
}

void test_normal_maps::OnScreenSizeChanged(int NewWidth, int NewHeight)
{
	SceneFramebuffer.Reload();
}
