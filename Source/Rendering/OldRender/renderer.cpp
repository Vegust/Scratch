#include "renderer.h"
#include "element_buffer.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/matrix.hpp"
#include "imgui.h"
#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer_layout.h"

#include <iostream>

void GlClearError() {
	while (glGetError() != GL_NO_ERROR)
		;
}

bool GlLogCall(const char* FunctionName, const char* FileName, int LineNumber) {
	if (GLenum Error = glGetError()) {
		std::cout << "OpenGl Error: (" << Error << "): " << FileName << ":" << LineNumber << ":"
				  << FunctionName << std::endl;
		CHECK(false);
		return false;
	}
	return true;
}

void renderer::Draw(
	const vertex_array& VertexArray,
	const element_buffer& IndexBuffer,
	const shader& Shader) {
	Shader.Bind();
	VertexArray.Bind();
	IndexBuffer.Bind();

	GL_CALL(glDrawElements(
		GL_TRIANGLES, static_cast<GLsizei>(IndexBuffer.GetCount()), GL_UNSIGNED_INT, nullptr));
}

void renderer::Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer::Draw(
	const vertex_array& VertexArray,
	const element_buffer& IndexBuffer,
	const shader& Shader,
	glm::mat4 Transform) const {
	Shader.Bind();
	VertexArray.Bind();
	IndexBuffer.Bind();
	Shader.Bind();
	Shader.SetUniform("u_MVP", CalcMVPForTransform(Transform));
	GL_CALL(glDrawElements(
		mDrawElementsMode, static_cast<GLsizei>(IndexBuffer.GetCount()), GL_UNSIGNED_INT, nullptr));
}

void renderer::DrawCubes(const shader& Shader, span<glm::mat4> Transforms) const {
	mCubeVAO.Bind();
	Shader.Bind();
	for (const auto& Transform : Transforms) {
		Shader.SetUniform("u_MVP", CalcMVPForTransform(Transform));
		GL_CALL(glDrawArrays(mDrawElementsMode, 0, 36));
	}
}

void renderer::DrawNormalCubes(const shader& Shader, span<glm::mat4> Transforms) const {
	mNormalCubeVAO.Bind();
	Shader.Bind();

	glm::mat4 View =
		glm::lookAt(mCameraPosition, mCameraPosition + mCameraDirection, mCameraUpVector);
	if (!mCustomCamera.expired()) {
		auto CameraHandle = mCustomCamera.lock();
		View = CameraHandle->GetViewTransform();
	}

	for (const auto& Transform : Transforms) {
		glm::mat4 ViewModel = View * Transform;
		Shader.SetUniform("u_ViewModel", ViewModel);
		Shader.SetUniform("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
		Shader.SetUniform("u_MVP", CalcMVPForTransform(Transform));
		GL_CALL(glDrawArrays(mDrawElementsMode, 0, 36));
	}
}

void renderer::DrawCubes(const phong_material& Material, span<glm::mat4> Transforms) const {
	mNormalCubeVAO.Bind();

	glm::mat4 View =
		glm::lookAt(mCameraPosition, mCameraPosition + mCameraDirection, mCameraUpVector);
	glm::vec3 CameraPos{};
	float CurrentFoV = mFoV;
	if (!mCustomCamera.expired()) {
		auto CameraHandle = mCustomCamera.lock();
		CurrentFoV = CameraHandle->FoV;
		View = CameraHandle->GetViewTransform();
		CameraPos = CameraHandle->Position;
	}

	Material.Bind();

	mActiveShader->Bind();
	mActiveShader->SetUniform("u_Material", Material);
	mActiveShader->SetUniform("u_InvertedView", glm::inverse(View));
	mActiveShader->SetUniform("u_Lights", "u_NumLights", mSceneLights, View);
	for (const auto& Transform : Transforms) {
		glm::mat4 ViewModel = View * Transform;
		mActiveShader->SetUniform("u_ViewModel", ViewModel);
		mActiveShader->SetUniform("u_Model", Transform);
		mActiveShader->SetUniform("u_CameraPos", CameraPos);
		mActiveShader->SetUniform(
			"u_NormalModelMatrix", glm::mat3(glm::transpose(glm::inverse(Transform))));
		mActiveShader->SetUniform(
			"u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
		mActiveShader->SetUniform("u_MVP", CalcMVPForTransform(Transform));
		GL_CALL(glDrawArrays(mDrawElementsMode, 0, 36));

		if (mNormals) {
			mNormalsShader.Bind();
			mNormalsShader.SetUniform("u_ViewModel", ViewModel);
			mNormalsShader.SetUniform(
				"u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
			glm::mat4 ProjectionMatrix =
				glm::perspective(glm::radians(CurrentFoV), mAspectRatio, 0.001f, 100.f);
			mNormalsShader.SetUniform("u_Projection", ProjectionMatrix);

			GL_CALL(glDrawArrays(mDrawElementsMode, 0, 36));
		}
	}
}

void renderer::Draw2(
	const vertex_array& VertexArray,
	const element_buffer& ElementBuffer,
	const phong_material& Material,
	const glm::mat4& Transform) const {
	VertexArray.Bind();
	ElementBuffer.Bind();
	Material.Bind();

	glm::mat4 View =
		glm::lookAt(mCameraPosition, mCameraPosition + mCameraDirection, mCameraUpVector);
	float CurrentFoV = mFoV;
	if (!mCustomCamera.expired()) {
		auto CameraHandle = mCustomCamera.lock();
		CurrentFoV = CameraHandle->FoV;
		View = CameraHandle->GetViewTransform();
	}
	glm::mat4 ViewModel = View * Transform;
	glm::mat4 ProjectionMatrix =
		glm::perspective(glm::radians(CurrentFoV), mAspectRatio, 0.001f, 100.f);

	mActiveShader->Bind();
	mActiveShader->SetUniform("u_Material", Material);
	mActiveShader->SetUniform("u_Lights", "u_NumLights", mSceneLights, View);
	mActiveShader->SetUniform("u_ViewModel", ViewModel);
	mActiveShader->SetUniform("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
	mActiveShader->SetUniform("u_MVP", CalcMVPForTransform(Transform));
	mActiveShader->SetUniform("u_Projection", ProjectionMatrix);
	mActiveShader->SetUniform("u_View", View);

	if (mInstanced) {
		glDrawElementsInstanced(
			mDrawElementsMode,
			static_cast<GLsizei>(VertexArray.mElementBufferSize),
			GL_UNSIGNED_INT,
			0,
			VertexArray.mInstanceCount);
	} else {
		GL_CALL(glDrawElements(
			mDrawElementsMode,
			static_cast<GLsizei>(VertexArray.mElementBufferSize),
			GL_UNSIGNED_INT,
			nullptr));

		if (mNormals) {
			mNormalsShader.Bind();
			mNormalsShader.SetUniform("u_ViewModel", ViewModel);
			mNormalsShader.SetUniform(
				"u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(ViewModel))));
			mNormalsShader.SetUniform("u_Projection", ProjectionMatrix);

			GL_CALL(glDrawElements(
				mDrawElementsMode,
				static_cast<GLsizei>(VertexArray.mElementBufferSize),
				GL_UNSIGNED_INT,
				nullptr));
		}
	}
}

void renderer::DrawFrameBuffer(const framebuffer& Framebuffer, bool Depth) {
	mScreenQuadVAO.Bind();
	mPostProcessShader.Bind();
	mPostProcessShader.SetUniform("u_Buffer", 0);
	mPostProcessShader.SetUniform("u_Depth", Depth);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(
		GL_TEXTURE_2D, Depth ? Framebuffer.mDepthStencilTextureId : Framebuffer.mColorTextureId);
	glDisable(GL_DEPTH_TEST);
	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
	glEnable(GL_DEPTH_TEST);
}

void renderer::DrawSkybox(const cubemap& Skybox) {
	glDepthFunc(GL_LEQUAL);
	mSkyboxVAO.Bind();
	mSkyboxShader.Bind();
	Skybox.Bind();
	mSkyboxShader.SetUniform("u_Cubemap", cubemap::CubemapSlot);
	float CurrentFoV = mFoV;
	if (!mCustomCamera.expired()) {
		auto CameraHandle = mCustomCamera.lock();
		CurrentFoV = CameraHandle->FoV;
	}
	glm::mat4 ProjectionMatrix =
		glm::perspective(glm::radians(CurrentFoV), mAspectRatio, 0.001f, 100.f);
	glm::mat4 View =
		glm::lookAt(mCameraPosition, mCameraPosition + mCameraDirection, mCameraUpVector);
	if (!mCustomCamera.expired()) {
		auto CameraHandle = mCustomCamera.lock();
		View = CameraHandle->GetViewTransform();
	}
	View = glm::mat4(glm::mat3(View));
	mSkyboxShader.SetUniform("u_ProjectionView", ProjectionMatrix * View);
	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
	glDepthFunc(GL_LESS);
}

void renderer::InitCubeVAO() {
	constexpr u32 NumVertices = 36;
	constexpr u32 ElementsPerVertex = 5;
	constexpr u32 SizeOfVertex = 5 * sizeof(float);
	constexpr array<float, NumVertices * ElementsPerVertex> Vertices = {
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

	mCubeVBO.SetData(Vertices.Data(), NumVertices * SizeOfVertex);
	vertex_buffer_layout VertexLayout{};
	VertexLayout.Push<float>(3);
	VertexLayout.Push<float>(2);
	mCubeVAO.AddBuffer(mCubeVBO, VertexLayout);
}

void renderer::InitNormalCubeVAO() {
	constexpr u32 NumVertices = 36;
	constexpr u32 ElementsPerVertex = 11;
	constexpr u32 SizeOfVertex = ElementsPerVertex * sizeof(float);
	constexpr array<float, NumVertices * ElementsPerVertex> Vertices = {
		// clang-format off
		// positions          // normals           // texture coords // tangents
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, -1.f,0.f,0.f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, -1.f,0.f,0.f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, -1.f,0.f,0.f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, -1.f,0.f,0.f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, -1.f,0.f,0.f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, -1.f,0.f,0.f,
		
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f, 1.f,0.f,0.f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f, 1.f,0.f,0.f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f, 1.f,0.f,0.f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f, 1.f,0.f,0.f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f, 1.f,0.f,0.f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f, 1.f,0.f,0.f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,0.0f,  -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,0.0f,  -1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,0.0f,  -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,0.0f,  -1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,0.0f,  -1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,0.0f,  -1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.f,0.f,0.f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.f,0.f,0.f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, 1.f,0.f,0.f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, 1.f,0.f,0.f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.f,0.f,0.f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.f,0.f,0.f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.f,0.f,0.f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 1.f,0.f,0.f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.f,0.f,0.f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.f,0.f,0.f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.f,0.f,0.f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 1.f,0.f,0.f

		// clang-format on
	};

	mNormalCubeVBO.SetData(Vertices.Data(), NumVertices * SizeOfVertex);
	vertex_buffer_layout VertexLayout{};
	VertexLayout.Push<float>(3);	// Position
	VertexLayout.Push<float>(3);	// Normals
	VertexLayout.Push<float>(2);	// UV
	VertexLayout.Push<float>(3);	// Tangents
	mNormalCubeVAO.AddBuffer(mNormalCubeVBO, VertexLayout);
}

void renderer::InitScreenQuadVAO() {
	constexpr array<float, 6 * 4> Vertices = {
		// clang-format off
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
		// clang-format on
	};

	mScreenQuadBVO.SetData(Vertices.Data(), Vertices.Size() * sizeof(float));
	vertex_buffer_layout VertexLayout{};
	VertexLayout.Push<float>(2);
	VertexLayout.Push<float>(2);
	mScreenQuadVAO.AddBuffer(mScreenQuadBVO, VertexLayout);
}

void renderer::InitSkyboxVAO() {
	constexpr array<float, 36 * 3> Vertices = {
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

	mSkyboxVBO.SetData(Vertices.Data(), Vertices.Size() * sizeof(float));
	vertex_buffer_layout VertexLayout{};
	VertexLayout.Push<float>(3);
	mSkyboxVAO.AddBuffer(mSkyboxVBO, VertexLayout);
}

glm::mat4 renderer::CalcMVPForTransform(const glm::mat4& Transform) const {
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(mFoV), mAspectRatio, 0.001f, 100.f);
	glm::mat4 ViewMatrix =
		glm::lookAt(mCameraPosition, mCameraPosition + mCameraDirection, mCameraUpVector);
	if (!mCustomCamera.expired()) {
		auto CameraHandle = mCustomCamera.lock();
		ProjectionMatrix =
			glm::perspective(glm::radians(CameraHandle->GetFoV()), mAspectRatio, 0.001f, 1000.f);
		ViewMatrix = CameraHandle->GetViewTransform();
	}

	glm::mat4 ModelMatrix = Transform;
	return ProjectionMatrix * ViewMatrix * ModelMatrix;
}

void renderer::Init() {
	InitCubeVAO();
	InitNormalCubeVAO();
	InitDefaultShaders();
	InitScreenQuadVAO();
	InitSkyboxVAO();
}

void renderer::InitDefaultShaders() {
	mPhongShader.Compile("Resources/Shaders/BasicShaded.shader");
	mPhongShader.Bind();
	mPhongShader.SetUniform("u_Unlit", false);
	mPhongShader.SetUniform("u_Depth", false);
	mOutlineShader.Compile("Resources/Shaders/Outline.shader");
	mPostProcessShader.Compile("Resources/Shaders/PostProcess.shader");
	mPostProcessShader.Bind();
	mPostProcessShader.SetUniform("u_Grayscale", false);
	mPostProcessShader.SetUniform("u_GammaCorrection", mGammaCorrection);
	mSkyboxShader.Compile("Resources/Shaders/Skybox.shader");
	mNormalsShader.Compile("Resources/Shaders/Normals.shader");
}

void renderer::ChangeViewMode(view_mode NewViewMode) {
	if (NewViewMode != mViewMode) {
		mViewMode = NewViewMode;
		mPhongShader.Bind();
		switch (mViewMode) {
			case view_mode::lit:
				mPhongShader.SetUniform("u_Unlit", false);
				mPhongShader.SetUniform("u_Depth", false);
				mDrawElementsMode = GL_TRIANGLES;
				break;
			case view_mode::unlit:
				mPhongShader.SetUniform("u_Unlit", true);
				mPhongShader.SetUniform("u_Depth", false);
				mDrawElementsMode = GL_TRIANGLES;
				break;
			case view_mode::wireframe:
				mPhongShader.SetUniform("u_Unlit", true);
				mPhongShader.SetUniform("u_Depth", false);
				mDrawElementsMode = GL_LINES;
				break;
			case view_mode::depth:
				mPhongShader.SetUniform("u_Unlit", true);
				mPhongShader.SetUniform("u_Depth", true);
				mDrawElementsMode = GL_TRIANGLES;
				break;
		}
	}
}

void renderer::UIViewModeControl() {
	constexpr array<view_mode, 4> Types = {
		view_mode::lit, view_mode::unlit, view_mode::wireframe, view_mode::depth};
	constexpr array<const char*, 4> Names = {"Lit", "Unlit", "Wireframe", "Depth"};
	if (ImGui::BeginCombo("View mode", Names[static_cast<u32>(mViewMode)])) {
		for (u32 i = 0; i < Types.Size(); ++i) {
			bool bIsSelected = mViewMode == Types[i];
			if (ImGui::Selectable(Names[i], bIsSelected)) {
				ChangeViewMode(Types[i]);
			}
		}
		ImGui::EndCombo();
	}
}

void renderer::UIPostProcessControl() {
	bool OldGrayscale = mGrayscale;
	ImGui::Checkbox("Grayscale", &mGrayscale);
	if (OldGrayscale != mGrayscale) {
		mPostProcessShader.SetUniform("u_Grayscale", mGrayscale);
	}
	ImGui::Checkbox("Draw normals", &mNormals);
	if (ImGui::SliderFloat("Gamma", &mGammaCorrection, 1.f, 5.f)) {
		mPostProcessShader.SetUniform("u_GammaCorrection", mGammaCorrection);
	}
}
