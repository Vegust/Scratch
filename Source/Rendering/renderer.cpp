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

void renderer::DrawCubes(const phong_material& Material, const std::vector<glm::mat4>& Transforms)
	const
{
	NormalCubeVAO.Bind();

	glm::mat4 View = glm::lookAt(CameraPosition, CameraPosition + CameraDirection, CameraUpVector);
	glm::vec3 CameraPos{};
	float CurrentFoV = FoV;
	if (!CustomCamera.expired())
	{
		auto CameraHandle = CustomCamera.lock();
		CurrentFoV = CameraHandle->FoV;
		View = CameraHandle->GetViewTransform();
		CameraPos = CameraHandle->Position;
	}
	
	Material.Bind();

	ActiveShader->Bind();
	ActiveShader->SetUniform("u_Material", Material);
	ActiveShader->SetUniform("u_Lights", "u_NumLights", SceneLights, View);
	for (const auto& Transform : Transforms)
	{
		glm::mat4 ViewModel = View * Transform;
		ActiveShader->SetUniform("u_ViewModel", ViewModel);
		ActiveShader->SetUniform("u_Model", Transform);
		ActiveShader->SetUniform("u_CameraPos", CameraPos);
		ActiveShader->SetUniform(
			"u_NormalModelMatrix", glm::mat3(glm::transpose(glm::inverse(Transform))));
		ActiveShader->SetUniform(
			"u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
		ActiveShader->SetUniform("u_MVP", CalcMVPForTransform(Transform));
		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
		
		if (bNormals)
		{
			NormalsShader.Bind();
			NormalsShader.SetUniform("u_ViewModel", ViewModel);
			NormalsShader.SetUniform("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
			glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(CurrentFoV), AspectRatio, 0.001f, 100.f);
			NormalsShader.SetUniform("u_Projection", ProjectionMatrix);
		
			GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
		}
	}
}

void renderer::Draw2(
	const vertex_array& VertexArray,
	const element_buffer& ElementBuffer,
	const phong_material& Material,
	const glm::mat4& Transform) const
{
	VertexArray.Bind();
	ElementBuffer.Bind();
	Material.Bind();

	glm::mat4 View = glm::lookAt(CameraPosition, CameraPosition + CameraDirection, CameraUpVector);
	float CurrentFoV = FoV;
	if (!CustomCamera.expired())
	{
		auto CameraHandle = CustomCamera.lock();
		CurrentFoV = CameraHandle->FoV;
		View = CameraHandle->GetViewTransform();
	}
	glm::mat4 ViewModel = View * Transform;
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(CurrentFoV), AspectRatio, 0.001f, 100.f);

	ActiveShader->Bind();
	ActiveShader->SetUniform("u_Material", Material);
	ActiveShader->SetUniform("u_Lights", "u_NumLights", SceneLights, View);
	ActiveShader->SetUniform("u_ViewModel", ViewModel);
	ActiveShader->SetUniform("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
	ActiveShader->SetUniform("u_MVP", CalcMVPForTransform(Transform));
	ActiveShader->SetUniform("u_Projection", ProjectionMatrix);
	ActiveShader->SetUniform("u_View", View);

	if (bInstanced)
	{
		glDrawElementsInstanced(
			GL_TRIANGLES, static_cast<GLsizei>(VertexArray.ElementBufferSize), GL_UNSIGNED_INT, 0, VertexArray.InstanceCount
		);
	}
	else
	{
		GL_CALL(glDrawElements(
			DrawElementsMode,
			static_cast<GLsizei>(VertexArray.ElementBufferSize),
			GL_UNSIGNED_INT,
			nullptr));
	
		if (bNormals)
		{
			NormalsShader.Bind();
			NormalsShader.SetUniform("u_ViewModel", ViewModel);
			NormalsShader.SetUniform("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
			NormalsShader.SetUniform("u_Projection", ProjectionMatrix);
		
			GL_CALL(glDrawElements(
				DrawElementsMode,
				static_cast<GLsizei>(VertexArray.ElementBufferSize),
				GL_UNSIGNED_INT,
				nullptr));
		}
	}
}

void renderer::DrawFrameBuffer(const framebuffer& Framebuffer)
{
	ScreenQuadVAO.Bind();
	PostProcessShader.Bind();
	PostProcessShader.SetUniform("u_Buffer", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Framebuffer.ColorTextureId);
	glDisable(GL_DEPTH_TEST);
	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
	glEnable(GL_DEPTH_TEST);
}

void renderer::DrawSkybox(const cubemap& Skybox)
{
	glDepthFunc(GL_LEQUAL);
	SkyboxVAO.Bind();
	SkyboxShader.Bind();
	Skybox.Bind();
	SkyboxShader.SetUniform("u_Cubemap", cubemap::CubemapSlot);
	float CurrentFoV = FoV;
	if (!CustomCamera.expired())
	{
		auto CameraHandle = CustomCamera.lock();
		CurrentFoV = CameraHandle->FoV;
	}
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(CurrentFoV), AspectRatio, 0.001f, 100.f);
	glm::mat4 View = glm::lookAt(CameraPosition, CameraPosition + CameraDirection, CameraUpVector);
	if (!CustomCamera.expired())
	{
		auto CameraHandle = CustomCamera.lock();
		View = CameraHandle->GetViewTransform();
	}
	View = glm::mat4(glm::mat3(View));
	SkyboxShader.SetUniform("u_ProjectionView", ProjectionMatrix * View);
	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
	glDepthFunc(GL_LESS);
}

void renderer::InitCubeVAO()
{
	constexpr uint32 NumVertices = 36;
	constexpr uint32 ElementsPerVertex = 5;
	constexpr uint32 SizeOfVertex = 5 * sizeof(float);
	constexpr std::array<float, NumVertices* ElementsPerVertex> Vertices = {
		// clang-format off
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		// Right face
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f
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
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		
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
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f

		// clang-format on
	};

	NormalCubeVBO.SetData(Vertices.data(), NumVertices * SizeOfVertex);
	vertex_buffer_layout VertexLayout{};
	VertexLayout.Push<float>(3);
	VertexLayout.Push<float>(3);
	VertexLayout.Push<float>(2);
	NormalCubeVAO.AddBuffer(NormalCubeVBO, VertexLayout);
}

void renderer::InitScreenQuadVAO()
{
	constexpr std::array<float, 6 * 4> Vertices = {
		// clang-format off
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
		// clang-format on
	};

	ScreenQuadBVO.SetData(Vertices.data(), Vertices.size() * sizeof(float));
	vertex_buffer_layout VertexLayout{};
	VertexLayout.Push<float>(2);
	VertexLayout.Push<float>(2);
	ScreenQuadVAO.AddBuffer(ScreenQuadBVO, VertexLayout);
}

void renderer::InitSkyboxVAO()
{
	constexpr std::array<float, 36 * 3> Vertices = {
		// clang-format off
 		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
		// clang-format on
	};

	SkyboxVBO.SetData(Vertices.data(), Vertices.size() * sizeof(float));
	vertex_buffer_layout VertexLayout{};
	VertexLayout.Push<float>(3);
	SkyboxVAO.AddBuffer(SkyboxVBO, VertexLayout);
}

glm::mat4 renderer::CalcMVPForTransform(const glm::mat4& Transform) const
{
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FoV), AspectRatio, 0.001f, 100.f);
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
	InitScreenQuadVAO();
	InitSkyboxVAO();
}

void renderer::InitDefaultShaders()
{
	PhongShader.Compile("Resources/Shaders/BasicShaded.shader");
	PhongShader.SetUniform("u_Unlit", false);
	PhongShader.SetUniform("u_Depth", false);
	OutlineShader.Compile("Resources/Shaders/Outline.shader");
	PostProcessShader.Compile("Resources/Shaders/PostProcess.shader");
	PostProcessShader.SetUniform("u_Grayscale", false);
	SkyboxShader.Compile("Resources/Shaders/Skybox.shader");
	NormalsShader.Compile("Resources/Shaders/Normals.shader");
}

void renderer::ChangeViewMode(view_mode NewViewMode)
{
	if (NewViewMode != ViewMode)
	{
		ViewMode = NewViewMode;
		PhongShader.Bind();
		switch (ViewMode)
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
	constexpr std::array<const char*, 4> Names = {"Lit", "Unlit", "Wireframe", "Depth"};
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

void renderer::UIPostProcessControl()
{
	bool bOldGrayscale = bGrayscale;
	ImGui::Checkbox("Grayscale", &bGrayscale);
	if (bOldGrayscale != bGrayscale)
	{
		PostProcessShader.SetUniform("u_Grayscale", bGrayscale);
	}
	ImGui::Checkbox("Draw normals", &bNormals);
}
