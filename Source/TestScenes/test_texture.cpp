#include "test_texture.h"

#include "Rendering/renderer.h"
#include "Rendering/vertex_buffer_layout.h"
#include "core_types.h"

#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "imgui.h"

REGISTER_TEST_SCENE(test_texture, "02 Texture")

test_texture::test_texture() {
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

	mTexture.Load("Resources/Textures/SlostestPhone.jpg");
	mTexture.Bind();

	mShader.Compile("Resources/Shaders/Basic.shader");
	mShader.Bind();
	mShader.SetUniform("u_Texture", 0);
	mShader.SetUniform("u_MVP", MVP);
}

test_texture::~test_texture() {
}

void test_texture::OnUpdate(float DeltaTime) {
	test_scene::OnUpdate(DeltaTime);
}

void test_texture::OnRender(renderer& Renderer) {
	test_scene::OnRender(Renderer);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	constexpr u32 WindowWidth = 1920;
	constexpr u32 WindowHeight = 1080;
	float AspectRatio = static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight);
	glm::mat4 ProjectionMatrix = glm::ortho(-AspectRatio, AspectRatio, -1.f, 1.f, -1.f, 1.f);
	glm::mat4 ViewMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(-0.5, 0.f, 0.f));

	glm::mat4 ModelMatrix = glm::translate(glm::mat4{1.0f}, mPic1Trans);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	mShader.Bind();
	mShader.SetUniform("u_MVP", MVP);
	renderer::Draw(mVertexArray, mIndexBuffer, mShader);

	ModelMatrix = glm::translate(glm::mat4{1.0f}, mPic2Trans);
	MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	mShader.Bind();
	mShader.SetUniform("u_MVP", MVP);
	renderer::Draw(mVertexArray, mIndexBuffer, mShader);
}

void test_texture::OnIMGuiRender() {
	test_scene::OnIMGuiRender();
	ImGui::SliderFloat3("Texture 1", &mPic1Trans.x, -1.f, 1.f);
	ImGui::SliderFloat3("Texture 2", &mPic2Trans.x, -1.f, 1.f);
}
