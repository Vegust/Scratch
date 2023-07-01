//
// Created by Vegust on 01.07.2023.
//

#include "test_shadowmaps.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

REGISTER_TEST_SCENE(test_shadowmaps, "09 Shadowmaps")

test_shadowmaps::test_shadowmaps()
{
	auto& Light = renderer::Get().SceneLights.emplace_back();
	Light.Type = light_type::directional;
	Light.Ambient = {0.2f, 0.2f, 0.2f};
	Light.Direction = {0.29, -0.58, -0.53};
	auto& PointLight = renderer::Get().SceneLights.emplace_back();
	PointLight.AttenuationRadius = 30.f;
	PointLight.Position = {0.f,2.f,-5.f};

	CubeMaterial.InitTextures(
		"Resources/Textures/Wood.png", 0, "Resources/Textures/DefaultSpecular.jpg", 1);

	SceneFramebuffer.Reload();

	Camera = std::make_shared<camera>();
	Camera->Position = glm::vec3{0.f, 1.f, 10.f};
	renderer::Get().CustomCamera = Camera;

	SetupCubeTransforms();
}

void test_shadowmaps::OnUpdate(float DeltaTime)
{
	UpdateDynamicCubeTransforms(DeltaTime);
}

void test_shadowmaps::OnRender(renderer& Renderer)
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	{
		SceneFramebuffer.Bind();
		glClearColor(0.8f, 0.8f, 1.0f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		Renderer.DrawCubes(CubeMaterial, StaticCubes);
		Renderer.DrawCubes(CubeMaterial, DynamicCubes);

		framebuffer::SetDefault();
	}

	Renderer.DrawFrameBuffer(SceneFramebuffer);
}

void test_shadowmaps::OnIMGuiRender()
{
	ImGui::SliderFloat("Slomo", &UpdateSpeed, 0.f, 2.f);
	if (ImGui::CollapsingHeader("Cubes"))
	{
		ImGui::InputFloat("Mat shininess", &CubeMaterial.Shininess);
		if (ImGui::Button("Add Cube"))
		{
			StaticCubes.push_back(glm::mat4(1.f));
		}

		if (StaticCubes.size() > 0)
		{
			ImGui::InputInt("Cube", &CubeEditIndex);
			CubeEditIndex = CubeEditIndex % StaticCubes.size();
			ImGui::InputFloat4("Column 4", glm::value_ptr(StaticCubes[CubeEditIndex][3]));
			ImGui::InputFloat4("Column 3", glm::value_ptr(StaticCubes[CubeEditIndex][2]));
			ImGui::InputFloat4("Column 2", glm::value_ptr(StaticCubes[CubeEditIndex][1]));
			ImGui::InputFloat4("Column 1", glm::value_ptr(StaticCubes[CubeEditIndex][0]));
		}
	}
	if (ImGui::CollapsingHeader("Lights"))
	{
		ImGui::InputInt("Light", &LightEditIndex);
		LightEditIndex = LightEditIndex % renderer::Get().SceneLights.size();
		renderer::Get().SceneLights[LightEditIndex].UIControlPanel("");
	}
}

void test_shadowmaps::OnScreenSizeChanged(int NewWidth, int NewHeight)
{
	SceneFramebuffer.Reload();
}

void test_shadowmaps::SetupCubeTransforms()
{
	// Static
	StaticCubes.clear();
	StaticCubes.push_back(glm::mat4(	// 0
		glm::vec4(20, 0, 0, 0),
		glm::vec4(0, 0.1, 0, 0),
		glm::vec4(0, 0, 30, 0),
		glm::vec4(0, -0.1, 0, 1)));
	StaticCubes.push_back(glm::mat4(	// 1
		glm::vec4(0.1, 0, 0, 0),
		glm::vec4(0, 10, 0, 0),
		glm::vec4(0, 0, 20, 0),
		glm::vec4(5, 4.9, 0, 1)));
	StaticCubes.push_back(glm::mat4(	// 2
		glm::vec4(0.1, 0, 0, 0),
		glm::vec4(0, 2, 0, 0),
		glm::vec4(0, 0, 10, 0),
		glm::vec4(-5, 0.9, 5, 1)));
	StaticCubes.push_back(glm::mat4(	// 3
		glm::vec4(0.1, 0, 0, 0),
		glm::vec4(0, 10, 0, 0),
		glm::vec4(0, 0, 10, 0),
		glm::vec4(-5, 4.9, -5, 1)));
	StaticCubes.push_back(glm::mat4(	// 4
		glm::vec4(10, 0, 0, 0),
		glm::vec4(0, 10, 0, 0),
		glm::vec4(0, 0, 0.1, 0),
		glm::vec4(0, 4.9, -10, 1)));
	StaticCubes.push_back(glm::mat4(	// 5
		glm::vec4(10, 0, 0, 0),
		glm::vec4(0, 0.1, 0, 0),
		glm::vec4(0, 0, 10, 0),
		glm::vec4(0, 9.9, -5, 1)));
	StaticCubes.push_back(glm::mat4(	// 6
		glm::vec4(10, 0, 0, 0),
		glm::vec4(0, 4, 0, 0),
		glm::vec4(0, 0, 0.1, 0),
		glm::vec4(0, 7.9, 0, 1)));
	StaticCubes.push_back(glm::mat4(	// 7
		glm::vec4(3, 0, 0, 0),
		glm::vec4(0, 3, 0, 0),
		glm::vec4(0, 0, 3, 0),
		glm::vec4(-3, 1.4, 3, 1)));
	StaticCubes.push_back(glm::mat4(	// 8
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(-3, 4, 3, 1)));
	StaticCubes.push_back(glm::mat4(	// 9
		glm::vec4(2, 0, 0, 0),
		glm::vec4(0, 2, 0, 0),
		glm::vec4(0, 0, 2, 0),
		glm::vec4(3, 0.9, -8, 1)));
	StaticCubes.push_back(glm::mat4(	// 10
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 7, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(-2, 6, -7, 1)));

	// Dynamic
	DynamicCubes.clear();
	DynamicCubes.push_back(glm::mat4(
		glm::vec4(2, 0, 0, 0),
		glm::vec4(0, 2, 0, 0),
		glm::vec4(0, 0, 2, 0),
		glm::vec4(-3, 2, 7.5, 1)));
	DynamicCubes.push_back(glm::mat4(
		glm::vec4(2, 0, 0, 0),
		glm::vec4(0, 2, 0, 0),
		glm::vec4(0, 0, 2, 0),
		glm::vec4(-2, 5, -4, 1)));
}

void test_shadowmaps::UpdateDynamicCubeTransforms(float DeltaTime)
{
	using namespace glm;
	AccTime += DeltaTime * UpdateSpeed;
	float Rotation = 0.5 * glm::pi<float>() * AccTime;
	float Translation = sin(AccTime) * 2.f;
	DynamicCubes[0] = scale(
		rotate(
			translate(mat4(1.f), vec3(-3, 2 + Translation, 7.5)), Rotation, vec3(0.5f, 1.f, 0.f)),
		vec3(2.f, 2.f, 2.f));
	DynamicCubes[1] = scale(
		rotate(
			translate(mat4(1.f), vec3(-2, 5 + Translation, -4)), Rotation, vec3(0.5f, 1.f, 0.f)),
		vec3(2.f, 2.f, 2.f));
}
