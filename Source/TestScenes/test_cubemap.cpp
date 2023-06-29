//
// Created by Vegust on 29.06.2023.
//

#include "test_cubemap.h"

#include "imgui.h"
#include <array>

REGISTER_TEST_SCENE(test_cubemap, "07 Cubemap")

test_cubemap::test_cubemap()
{
	CubeMaterial.InitTextures(
		"Resources/Textures/Box/BoxDiffuse.png", 0, "Resources/Textures/Box/BoxSpecular.png", 1);
	
	Skybox.Load({
		"Resources/Textures/Skybox_Mountains"
	});
	
	MirrorShader.Compile("Resources/Shaders/Mirror.shader");
	MirrorShader.SetUniform("u_Cubemap", cubemap::CubemapSlot);
	
	GlassShader.Compile("Resources/Shaders/Glass.shader");
	GlassShader.SetUniform("u_Cubemap", cubemap::CubemapSlot);
	
	Camera = std::make_shared<camera>();
	Camera->Position = glm::vec3{0.f, 0.f, 1.5f};
	renderer::Get().CustomCamera = Camera;
	
	auto& Light = renderer::Get().SceneLights.emplace_back();
	Light.Type = light_type::directional;
	Light.Ambient = glm::vec3{0.5f, 0.5f, 0.5f};
	Light.Diffuse = glm::vec3{0.5f, 0.5f, 0.5};
}

void test_cubemap::OnRender(renderer& Renderer)
{
	test_scene::OnRender(Renderer);
	
	glClearColor(0.1f, 0.2f, 0.1f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	switch (MatType)
	{
		case mat_default:
			break;
		case mat_mirror:
			Renderer.SetActiveShader(&MirrorShader);
			MirrorShader.Bind();
			Skybox.Bind();
			MirrorShader.SetUniform("u_Cubemap", cubemap::CubemapSlot);
			break;
		case mat_glass:
			Renderer.SetActiveShader(&GlassShader);
			GlassShader.Bind();
			Skybox.Bind();
			GlassShader.SetUniform("u_Cubemap", cubemap::CubemapSlot);
			GlassShader.SetUniform("u_RefractiveIndex", RefractiveIndex);
			break;
	}

	std::vector<glm::mat4> CubeTransforms;
	CubeTransforms.push_back(glm::translate(glm::mat4{1.f}, CubePosition));
	Renderer.DrawCubes(CubeMaterial, CubeTransforms);
	Renderer.SetActiveShader(&Renderer.PhongShader);
	
	Renderer.DrawSkybox(Skybox);
}

void test_cubemap::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
	
	constexpr std::array<mat_type, 3> Types = {
		mat_default, mat_mirror, mat_glass};
	constexpr std::array<const char*, 3> TypeNames = {
		"default", "mirror", "glass"};
	
	if (ImGui::BeginCombo("Mat type", TypeNames[MatType]))
	{
		for (uint32 i = 0; i < Types.size(); ++i)
		{
			bool bIsSelected = MatType == Types[i];
			if (ImGui::Selectable(TypeNames[Types[i]], bIsSelected))
			{
				MatType = Types[i];
			}
		}
		ImGui::EndCombo();
	}
	
	if (MatType == mat_type::mat_glass)
	{
		ImGui::SliderFloat("Refractive Index", &RefractiveIndex, 0.0f, 5.0f);
	}
	
	renderer::Get().SceneLights[0].UIControlPanel("");
}
