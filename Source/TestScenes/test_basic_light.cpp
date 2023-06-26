//
// Created by Vegust on 25.06.2023.
//

#include "test_basic_light.h"

#include "core_types.h"
#include "glm/matrix.hpp"

#include <memory>
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

	std::vector<glm::mat4> Transforms;
	glm::mat4 ModelTransform = glm::rotate(
		glm::translate(glm::mat4{1.0f}, CubePosition), CurrentRotation, glm::vec3(0.5f, 1.f, 0.f));
	Transforms.push_back(ModelTransform);
	glm::mat4 ViewModel = View * ModelTransform;

	Shader->Bind();
	Shader->SetUniform("u_Material", CubeMaterial);
	Shader->SetUniform("u_ViewModel", ViewModel);
	Shader->SetUniform("u_Light", Light, View);
	Shader->SetUniform("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
	Renderer.DrawNormalCubes(*Shader, Transforms);

	std::vector<glm::mat4> LightTransforms;
	glm::mat4 LightTransform = glm::scale(
		glm::rotate(
			glm::translate(glm::mat4{1.0f}, Light.Position),
			CurrentRotation,
			glm::vec3(-0.5f, 1.f, 0.f)),
		glm::vec3(0.2f, 0.2f, 0.2f));
	LightTransforms.push_back(LightTransform);

	LightShader->Bind();
	LightShader->SetUniform("u_Light", Light, View);
	Renderer.DrawNormalCubes(*LightShader, LightTransforms);
}

void test_basic_light::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
	ImGui::SliderFloat3("Cube Position", glm::value_ptr(CubePosition), -5.f, 5.f);
	ImGui::ColorEdit3("Cube Diffuse Color", glm::value_ptr(CubeMaterial.Diffuse));
	ImGui::ColorEdit3("Cube Ambient Color", glm::value_ptr(CubeMaterial.Ambient));
	ImGui::ColorEdit3("Cube Specular Color", glm::value_ptr(CubeMaterial.Specular));
	ImGui::SliderFloat("Cube Shininess", &CubeMaterial.Shininess, 2.f, 256.f);
	ImGui::SliderFloat3("Light Position", glm::value_ptr(Light.Position), -5.f, 5.f);
	ImGui::ColorEdit3("Light Diffuse Color", glm::value_ptr(Light.Diffuse));
	ImGui::ColorEdit3("Light Ambient Color", glm::value_ptr(Light.Ambient));
	ImGui::ColorEdit3("Light Specular Color", glm::value_ptr(Light.Specular));
	ImGui::Checkbox("Rotating", &bRotating);
}
