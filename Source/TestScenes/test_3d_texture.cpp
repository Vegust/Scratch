#include "test_3d_texture.h"

#include "glm/ext/scalar_constants.hpp"

#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "Rendering/renderer.h"
#include "Rendering/vertex_buffer_layout.h"

REGISTER_TEST_SCENE(test_3d_texture, "03 3D Texture")

test_3d_texture::test_3d_texture() {
	constexpr u32 NumTriangles = 2;
	constexpr u32 NumVertices = 4;
	constexpr u32 SizeOfVertex = 4 * sizeof(float);
	array<float, NumVertices * SizeOfVertex> Positions = {
		// clang-format off
		 0.4f, -0.4f, 1.f, 0.f,
		-0.4f, -0.4f, 0.f, 0.f,
		 0.4f,	0.4f, 1.f, 1.f,
		-0.4f,	0.4f, 0.f, 1.f
		// clang-format on
	};
	array<u32, NumTriangles * 3> Indices = {2, 1, 0, 1, 2, 3};

	constexpr u32 WindowWidth = 1920;
	constexpr u32 WindowHeight = 1080;
	constexpr float InitialAspectRatio = static_cast<float>(WindowWidth) / WindowHeight;
	glm::mat4 ProjectionMatrix =
		glm::ortho(-InitialAspectRatio, InitialAspectRatio, -1.f, 1.f, -1.f, 1.f);
	glm::mat4 ViewMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(-0.5, 0.f, 0.f));
	glm::mat4 ModelMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(0.5, 0.f, 0.f));
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	mVertexBuffer.SetData(Positions.Data(), NumVertices * SizeOfVertex);
	vertex_buffer_layout VertexLayout{};
	VertexLayout.Push<float>(2);
	VertexLayout.Push<float>(2);
	mVertexArray.AddBuffer(mVertexBuffer, VertexLayout);

	mIndexBuffer.SetData(Indices.Data(), Indices.Size());

	mTexture.Load("Resources/Textures/Wall.jpg");
	mTexture.Bind(0);

	mShader.Compile("Resources/Shaders/Basic.shader");
	mShader.Bind();
	mShader.SetUniform("u_Texture", 0);
	mShader.SetUniform("u_MVP", MVP);
}

test_3d_texture::~test_3d_texture() {
}

void test_3d_texture::OnUpdate(float DeltaTime) {
	test_scene::OnUpdate(DeltaTime);
}

void test_3d_texture::OnRender(renderer& Renderer) {
	test_scene::OnRender(Renderer);
	glClearColor(1.f, 1.f, 1.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	constexpr u32 WindowWidth = 1920;
	constexpr u32 WindowHeight = 1080;
	float AspectRatio = static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight);
	// glm::mat4 ProjectionMatrix = glm::ortho(-AspectRatio, AspectRatio, -1.f, 1.f, 0.f, 2.f);
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(120.0f), AspectRatio, 0.001f, 2.f);
	glm::mat4 ViewMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(0.0, 0.f, -1.f));

	glm::mat4 ModelMatrix = glm::rotate(
		glm::rotate(
			glm::rotate(
				glm::translate(glm::mat4{1.0f}, mPic1Trans), mPic1Rot.x, glm::vec3(1.f, 0.f, 0.f)),
			mPic1Rot.y,
			glm::vec3(0.f, 1.f, 0.f)),
		mPic1Rot.z,
		glm::vec3(0.f, 0.f, 1.f));
	// glm::mat4 ModelMatrix = glm::translate(glm::mat4{1.0f}, Pic1Trans);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	mShader.Bind();
	mShader.SetUniform("u_MVP", MVP);
	renderer::Draw(mVertexArray, mIndexBuffer, mShader);
}

void test_3d_texture::OnIMGuiRender() {
	test_scene::OnIMGuiRender();
	constexpr float Pi = glm::pi<float>();
	ImGui::SliderFloat3("Texture translation", glm::value_ptr(mPic1Trans), -1.f, 1.f);
	ImGui::SliderFloat3("Texture rotation", glm::value_ptr(mPic1Rot), -Pi, Pi);
}
