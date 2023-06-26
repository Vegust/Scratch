//
// Created by Vegust on 25.06.2023.
//

#include "test_basic_light.h"

#include "core_types.h"
#include "glm/matrix.hpp"

#include <memory>
#include <string>
#include <vector>

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
SCRATCH_DISABLE_WARNINGS_END()

#include "Rendering/renderer.h"
#include "Rendering/vertex_buffer_layout.h"

REGISTER_TEST_SCENE(test_basic_light, "05 Basic (Phong?) Light")

test_basic_light::test_basic_light()
{
	Shader = std::make_unique<shader>("Resources/Shaders/BasicShaded.shader");
	LightShader = std::make_unique<shader>("Resources/Shaders/BasicLight.shader");

	CubeMaterial.InitTextures(
		"Resources/Textures/Box/BoxDiffuse.png", 0, "Resources/Textures/Box/BoxSpecular.png", 1);
	//"Resources/Textures/OpenGL_Logo.png",
	// 2);

	Camera = std::make_shared<camera>();
	Camera->Position = glm::vec3{0.f, 0.f, 1.5f};
	renderer::Get().CustomCamera = Camera;
}

test_basic_light::~test_basic_light()
{
}

void test_basic_light::OnUpdate(float DeltaTime)
{
	test_scene::OnUpdate(DeltaTime);
	if (bRotating)
	{
		CurrentRotation += glm::pi<float>() / 2.f * DeltaTime;
	}
}

void test_basic_light::OnRender(renderer& Renderer)
{
	test_scene::OnRender(Renderer);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const glm::mat4 View = Camera->GetViewTransform();

	std::srand(0);
	std::vector<glm::mat4> Transforms;
	for (int i = 0; i < 10; ++i)
	{
		glm::vec3 RandomOffset = glm::vec3{
			(std::rand() % 2000 - 1000) / 200.f,
			(std::rand() % 2000 - 1000) / 200.f,
			(std::rand() % 2000 - 2000) / 200.f};
		glm::vec3 Position = i == 0 ? CubePosition : CubePosition + RandomOffset;
		glm::mat4 ModelTransform = glm::rotate(
			glm::translate(glm::mat4{1.0f}, Position),
			CurrentRotation + static_cast<float>(i) * glm::pi<float>() / 10.f,
			glm::vec3(0.5f, 1.f, 0.f));
		Transforms.push_back(ModelTransform);
	}

	Shader->Bind();
	Shader->SetUniform("u_Material", CubeMaterial);
	Shader->SetUniform("u_Lights", "u_NumLights", Lights, View);
	Renderer.DrawNormalCubes(*Shader, Transforms);

	std::vector<glm::mat4> LightTransforms;
	for (const auto& Light : Lights)
	{
		glm::mat4 LightTransform = glm::scale(
			glm::rotate(
				glm::translate(glm::mat4{1.0f}, Light.Position),
				CurrentRotation,
				glm::vec3(-0.5f, 1.f, 0.f)),
			glm::vec3(0.2f, 0.2f, 0.2f));
		LightTransforms.push_back(LightTransform);
		LightShader->Bind();
		// This won't work properly for light cubes color right now but i don't care tbh
		LightShader->SetUniform("u_Light", Light, View);
	}

	Renderer.DrawNormalCubes(*LightShader, LightTransforms);
}

void test_basic_light::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
	ImGui::SliderFloat3("Cube Position", glm::value_ptr(CubePosition), -5.f, 5.f);
	ImGui::SliderFloat("Cube Shininess", &CubeMaterial.Shininess, 2.f, 256.f);
	for (uint64 i = 0; i < Lights.size(); ++i)
	{
		Lights[i].UIControlPanel(std::to_string(i));
	}
	if (Lights.size() < 100 && ImGui::Button("Add Light"))
	{
		Lights.emplace_back();
	}
	if (Lights.size() > 0 && ImGui::Button("Remove Light"))
	{
		Lights.pop_back();
	}
	
	ImGui::Checkbox("Rotating", &bRotating);
}
