//
// Created by Vegust on 21.06.2023.
//

#include "renderer.h"

#include "element_buffer.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/matrix.hpp"
#include "imgui.h"
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
		DrawElementsMode, static_cast<GLsizei>(IndexBuffer.GetCount()), GL_UNSIGNED_INT, nullptr));
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

void renderer::DrawPhong(
	const vertex_array& VertexArray,
	const element_buffer& ElementBuffer,
	const phong_material& Material,
	const glm::mat4& Transform) const
{
	VertexArray.Bind();
	ElementBuffer.Bind();
	Material.Bind();

	glm::mat4 View = glm::lookAt(CameraPosition, CameraPosition + CameraDirection, CameraUpVector);
	if (!CustomCamera.expired())
	{
		auto CameraHandle = CustomCamera.lock();
		View = CameraHandle->GetViewTransform();
	}
	glm::mat4 ViewModel = View * Transform;

	PhongShader.Bind();
	PhongShader.SetUniform("u_Material", Material);
	PhongShader.SetUniform("u_Lights", "u_NumLights", SceneLights, View);
	PhongShader.SetUniform("u_ViewModel", ViewModel);
	PhongShader.SetUniform("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
	PhongShader.SetUniform("u_MVP", CalcMVPForTransform(Transform));
	
	GL_CALL(glDrawElements(
		DrawElementsMode,
		static_cast<GLsizei>(VertexArray.ElementBufferSize),
		GL_UNSIGNED_INT,
		nullptr));
}

void renderer::InitCubeVAO()
{
	constexpr uint32 NumVertices = 36;
	constexpr uint32 ElementsPerVertex = 5;
	constexpr uint32 SizeOfVertex = 5 * sizeof(float);
	constexpr std::array<float, NumVertices* ElementsPerVertex> Vertices = {
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
	constexpr std::array<float, NumVertices* ElementsPerVertex> Vertices = {
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
	PhongShader.SetUniform("u_Unlit", false);
	PhongShader.SetUniform("u_Depth", false);
}

void renderer::ChangeViewMode(view_mode NewViewMode)
{
	if (NewViewMode != ViewMode)
	{
		ViewMode = NewViewMode;
		PhongShader.Bind();
		switch(ViewMode)
		{
			case view_mode::lit:
				PhongShader.SetUniform("u_Unlit", false);
				PhongShader.SetUniform("u_Depth", false);
				DrawElementsMode = GL_TRIANGLES;
				break;
			case view_mode::unlit:
				PhongShader.SetUniform("u_Unlit", true);
				PhongShader.SetUniform("u_Depth", false);
				DrawElementsMode = GL_TRIANGLES;
				break;
			case view_mode::wireframe:
				PhongShader.SetUniform("u_Unlit", true);
				PhongShader.SetUniform("u_Depth", false);
				DrawElementsMode = GL_LINES;
				break;
			case view_mode::depth:
				PhongShader.SetUniform("u_Unlit", true);
				PhongShader.SetUniform("u_Depth", true);
				DrawElementsMode = GL_TRIANGLES;
				break;
		}
	}
}

void renderer::UIViewModeControl()
{
	constexpr std::array<view_mode, 4> Types = {
		view_mode::lit, view_mode::unlit, view_mode::wireframe, view_mode::depth};
	constexpr std::array<const char*, 4> Names = {
		"Lit", "Unlit", "Wireframe", "Depth"};
	if (ImGui::BeginCombo("View mode", Names[static_cast<uint32>(ViewMode)]))
	{
		for (uint32 i = 0; i < Types.size(); ++i)
		{
			bool bIsSelected = ViewMode == Types[i];
			if (ImGui::Selectable(Names[i], bIsSelected))
			{
				ChangeViewMode(Types[i]);
			}
		}
		ImGui::EndCombo();
	}
}
