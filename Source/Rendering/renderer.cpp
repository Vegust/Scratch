//
// Created by Vegust on 21.06.2023.
//

#include "renderer.h"

#include "element_buffer.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/matrix.hpp"
#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer_layout.h"

#include <array>
#include <iostream>

void GlClearError()
{
	while (glGetError() != GL_NO_ERROR)
		;
}

bool GlLogCall(const char* FunctionName, const char* FileName, int LineNumber)
{
	if (GLenum Error = glGetError())
	{
		std::cout << "OpenGl Error: (" << Error << "): " << FileName << ":" << LineNumber << ":"
				  << FunctionName << std::endl;
		return false;
	}
	return true;
}

void renderer::Draw(
	const vertex_array& VertexArray,
	const element_buffer& IndexBuffer,
	const shader& Shader)
{
	Shader.Bind();
	VertexArray.Bind();
	IndexBuffer.Bind();

	GL_CALL(glDrawElements(
		GL_TRIANGLES, static_cast<GLsizei>(IndexBuffer.GetCount()), GL_UNSIGNED_INT, nullptr));
}

void renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer::Draw(
	const vertex_array& VertexArray,
	const element_buffer& IndexBuffer,
	const shader& Shader,
	glm::mat4 Transform) const
{
	Shader.Bind();
	VertexArray.Bind();
	IndexBuffer.Bind();
	Shader.Bind();
	Shader.SetUniform("u_MVP", CalcMVPForTransform(Transform));
	GL_CALL(glDrawElements(
		GL_TRIANGLES, static_cast<GLsizei>(IndexBuffer.GetCount()), GL_UNSIGNED_INT, nullptr));
}

void renderer::DrawCubes(const shader& Shader, const std::vector<glm::mat4>& Transforms) const
{
	CubeVAO.Bind();
	Shader.Bind();
	for (const auto& Transform : Transforms)
	{
		Shader.SetUniform("u_MVP", CalcMVPForTransform(Transform));
		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
	}
}

void renderer::DrawNormalCubes(const shader& Shader, const std::vector<glm::mat4>& Transforms) const
{
	NormalCubeVAO.Bind();
	Shader.Bind();

	glm::mat4 View = glm::lookAt(CameraPosition, CameraPosition + CameraDirection, CameraUpVector);
	if (!CustomCamera.expired())
	{
		auto CameraHandle = CustomCamera.lock();
		View = CameraHandle->GetViewTransform();
	}

	for (const auto& Transform : Transforms)
	{
		glm::mat4 ViewModel = View * Transform;
		Shader.SetUniform("u_ViewModel", ViewModel);
		Shader.SetUniform("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
		Shader.SetUniform("u_MVP", CalcMVPForTransform(Transform));
		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
	}
}

void renderer::DrawPhong(const vertex_array& VertexArray, const phong_material& Material, const glm::mat4& Transform) const
{
	VertexArray.Bind();
	PhongShader.Bind();
	
	glm::mat4 View = glm::lookAt(CameraPosition, CameraPosition + CameraDirection, CameraUpVector);
	if (!CustomCamera.expired())
	{
		auto CameraHandle = CustomCamera.lock();
		View = CameraHandle->GetViewTransform();
	}
	
	PhongShader.Bind();
	PhongShader.SetUniform("u_Material", Material);
	PhongShader.SetUniform("u_Lights", "u_NumLights", SceneLights, View);
	
	glm::mat4 ViewModel = View * Transform;
	PhongShader.SetUniform("u_ViewModel", ViewModel);
	PhongShader.SetUniform("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
	PhongShader.SetUniform("u_MVP", CalcMVPForTransform(Transform));
	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
}

void renderer::InitCubeVAO()
{
	constexpr uint32 NumVertices = 36;
	constexpr uint32 ElementsPerVertex = 5;
	constexpr uint32 SizeOfVertex = 5 * sizeof(float);

	std::array<float, NumVertices* ElementsPerVertex> Vertices = {
		// clang-format off
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		// clang-format on
	};

	CubeVBO.SetData(Vertices.data(), NumVertices * SizeOfVertex);
	vertex_buffer_layout VertexLayout{};
	VertexLayout.Push<float>(3);
	VertexLayout.Push<float>(2);
	CubeVAO.AddBuffer(CubeVBO, VertexLayout);
}

void renderer::InitNormalCubeVAO()
{
	constexpr uint32 NumVertices = 36;
	constexpr uint32 ElementsPerVertex = 8;
	constexpr uint32 SizeOfVertex = ElementsPerVertex * sizeof(float);

	std::array<float, NumVertices* ElementsPerVertex> Vertices = {
		// clang-format off
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
		// clang-format on
	};

	NormalCubeVBO.SetData(Vertices.data(), NumVertices * SizeOfVertex);
	vertex_buffer_layout VertexLayout{};
	VertexLayout.Push<float>(3);
	VertexLayout.Push<float>(3);
	VertexLayout.Push<float>(2);
	NormalCubeVAO.AddBuffer(NormalCubeVBO, VertexLayout);
}

glm::mat4 renderer::CalcMVPForTransform(const glm::mat4& Transform) const
{
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FoV), AspectRatio, 0.001f, 1000.f);
	glm::mat4 ViewMatrix =
		glm::lookAt(CameraPosition, CameraPosition + CameraDirection, CameraUpVector);
	if (!CustomCamera.expired())
	{
		auto CameraHandle = CustomCamera.lock();
		ProjectionMatrix =
			glm::perspective(glm::radians(CameraHandle->GetFoV()), AspectRatio, 0.001f, 1000.f);
		ViewMatrix = CameraHandle->GetViewTransform();
	}

	glm::mat4 ModelMatrix = Transform;
	return ProjectionMatrix * ViewMatrix * ModelMatrix;
}

void renderer::Init()
{
	InitCubeVAO();
	InitNormalCubeVAO();
	InitDefaultShaders();
}

void renderer::InitDefaultShaders()
{
	PhongShader.Compile("Resources/Shaders/BasicShaded.shader");
}
