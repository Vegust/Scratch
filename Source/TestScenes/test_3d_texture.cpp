//
// Created by Vegust on 24.06.2023.
//

#include "test_3d_texture.h"

#include "Rendering/vertex_array.h"
#include "core_types.h"
#include "glm/ext/scalar_constants.hpp"

#include <array>
#include <memory>

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
SCRATCH_DISABLE_WARNINGS_END()

#include "Rendering/renderer.h"
#include "Rendering/texture.h"
#include "Rendering/vertex_buffer_layout.h"

REGISTER_TEST_SCENE(test_3d_texture, "03 3D Texture")

test_3d_texture::test_3d_texture()
{
	constexpr uint32 NumTriangles = 2;
	constexpr uint32 NumVertices = 4;
	constexpr uint32 SizeOfVertex = 4 * sizeof(float);
	std::array<float, NumVertices* SizeOfVertex> Positions = {
		// clang-format off
		 0.4f, -0.4f, 1.f, 0.f,
		-0.4f, -0.4f, 0.f, 0.f,
		 0.4f,	0.4f, 1.f, 1.f,
		-0.4f,	0.4f, 0.f, 1.f
		// clang-format on
	};
	std::array<uint32, NumTriangles* 3> Indices = {0, 1, 2, 1, 2, 3};

	VertexArray = std::make_unique<vertex_array>();

	constexpr uint32 WindowWidth = 1920;
	constexpr uint32 WindowHeight = 1080;
	constexpr float InitialAspectRatio = static_cast<float>(WindowWidth) / WindowHeight;
	glm::mat4 ProjectionMatrix =
		glm::ortho(-InitialAspectRatio, InitialAspectRatio, -1.f, 1.f, -1.f, 1.f);
	glm::mat4 ViewMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(-0.5, 0.f, 0.f));
	glm::mat4 ModelMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(0.5, 0.f, 0.f));
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	VertexBuffer = std::make_unique<vertex_buffer>(Positions.data(), NumVertices * SizeOfVertex);
	vertex_buffer_layout VertexLayout{};
	VertexLayout.Push<float>(2);
	VertexLayout.Push<float>(2);
	VertexArray->AddBuffer(*VertexBuffer, VertexLayout);

	IndexBuffer = std::make_unique<index_buffer>(Indices.data(), Indices.size());

	texture Texture{"Resources/Textures/Wall.jpg"};
	Texture.Bind();

	Shader = std::make_unique<shader>("Resources/Shaders/Basic.shader");
	Shader->Bind();
	Shader->SetUniform("u_Texture", 0);
	Shader->SetUniform("u_MVP", MVP);
}

test_3d_texture::~test_3d_texture()
{
}

void test_3d_texture::OnUpdate(float DeltaTime)
{
	test_scene::OnUpdate(DeltaTime);
}

void test_3d_texture::OnRender(renderer& Renderer)
{
	test_scene::OnRender(Renderer);
	glClearColor(1.f, 1.f, 1.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	constexpr uint32 WindowWidth = 1920;
	constexpr uint32 WindowHeight = 1080;
	float AspectRatio = static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight);
	//glm::mat4 ProjectionMatrix = glm::ortho(-AspectRatio, AspectRatio, -1.f, 1.f, 0.f, 2.f);
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(120.0f), AspectRatio, 0.001f, 2.f);
	glm::mat4 ViewMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(0.0, 0.f, -1.f));

	glm::mat4 ModelMatrix = glm::rotate(
		glm::rotate(
			glm::rotate(
				glm::translate(glm::mat4{1.0f}, Pic1Trans), Pic1Rot.x, glm::vec3(1.f, 0.f, 0.f)),
			Pic1Rot.y,
			glm::vec3(0.f, 1.f, 0.f)),
		Pic1Rot.z,
		glm::vec3(0.f, 0.f, 1.f));
	//glm::mat4 ModelMatrix = glm::translate(glm::mat4{1.0f}, Pic1Trans);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	Shader->Bind();
	Shader->SetUniform("u_MVP", MVP);
	renderer::Draw(*VertexArray, *IndexBuffer, *Shader);
}

void test_3d_texture::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
	constexpr float Pi = glm::pi<float>();
	ImGui::SliderFloat3("Texture translation", glm::value_ptr(Pic1Trans), -1.f, 1.f);
	ImGui::SliderFloat3("Texture rotation", glm::value_ptr(Pic1Rot), -Pi, Pi);
}
