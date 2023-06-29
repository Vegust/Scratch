//
// Created by Vegust on 29.06.2023.
//

#include "test_cubemap.h"

REGISTER_TEST_SCENE(test_cubemap, "07 Cubemap")

test_cubemap::test_cubemap()
{
	CubeMaterial.InitTextures(
		"Resources/Textures/Box/BoxDiffuse.png", 0, "Resources/Textures/Box/BoxSpecular.png", 1);
	
	Skybox.Load({
		"Resources/Textures/Skybox_Mountains"
	});
	
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
	
	Renderer.DrawSkybox(Skybox);
	
	std::vector<glm::mat4> CubeTransforms;
	CubeTransforms.push_back(glm::translate(glm::mat4{1.f}, CubePosition));
	Renderer.DrawCubes(CubeMaterial, CubeTransforms);
}

void test_cubemap::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
	renderer::Get().SceneLights[0].UIControlPanel("");
}
