#include "old_rebderer.h"
#include "element_buffer.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/matrix.hpp"
#include "imgui.h"
#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer_layout.h"
#include "Logger/logger.h"

void GlClearError() {
	while (glGetError() != GL_NO_ERROR)
		;
}

bool GlLogCall(const char* FunctionName, const char* FileName, int LineNumber) {
	if (GLenum Error = glGetError()) {
		logger::Log("OpenGL Error (%s): %s:%s:%s", Error, FileName, LineNumber, FunctionName);
		CHECK(false);
		return false;
	}
	return true;
}

void old_rebderer::OnScreenSizeChanged(u32 NewWidth, u32 NewHeight) {
	glViewport(0, 0, (s32) NewWidth, (s32) NewHeight);
	mAspectRatio = static_cast<float>(NewWidth) / static_cast<float>(NewHeight);
	mCurrentHeight = NewHeight;
	mCurrentWidth = NewWidth;
}

void old_rebderer::DrawCubes(const phong_material& Material, span<glm::mat4> Transforms) {
	mNormalCubeVAO.Bind();
	glm::mat4 View = mCamera.GetViewTransform();
	glm::mat4 Projection = glm::perspective(glm::radians(mCamera.mFoV), mAspectRatio, 0.001f, 100.f);
	Material.Bind();
	mActiveShader->Bind();
	// global
	if (GlobalUBO.mProjection != Projection) {
		GlobalUBO.mProjection = Projection;
		UpdateGlobalUBO();
	}
	// view
	mActiveShader->SetUniform("u_View", View);
	mActiveShader->SetUniform("u_InvertedView", glm::inverse(View));
	// material
	mActiveShader->SetUniform("u_Material", Material);
	// lights
	mActiveShader->SetUniform("u_NumLights", 2);
	UpdateLightsSSBO();
	// mActiveShader->SetUniform("u_Lights", "u_NumLights", mSceneLights, View);
	for (const auto& Transform : Transforms) {
		// model
		mActiveShader->SetUniform("u_Model", Transform);
		mActiveShader->SetUniform("u_ModelNormal", glm::transpose(glm::inverse(Transform)));
		GL_CALL(glDrawArrays(mDrawElementsMode, 0, 36));
	}
}

void old_rebderer::DrawFrameBuffer(const framebuffer& Framebuffer, bool Depth) {
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

void old_rebderer::DrawSkybox(const cubemap& Skybox) {
	glDepthFunc(GL_LEQUAL);
	mSkyboxVAO.Bind();
	mSkyboxShader.Bind();
	Skybox.Bind();
	mSkyboxShader.SetUniform("u_Cubemap", cubemap::CubemapSlot);
	glm::mat4 View = mCamera.GetViewTransform();
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(mFoV), mAspectRatio, 0.001f, 100.f);
	View = glm::mat4(glm::mat3(View));
	mSkyboxShader.SetUniform("u_ProjectionView", ProjectionMatrix * View);
	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
	glDepthFunc(GL_LESS);
}

void old_rebderer::InitCubeVAO() {
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

void old_rebderer::InitNormalCubeVAO() {
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

void old_rebderer::InitScreenQuadVAO() {
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

void old_rebderer::InitSkyboxVAO() {
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

void old_rebderer::Init(u32 WindowWidth, u32 WindowHeight) {
	mCurrentHeight = WindowHeight;
	mCurrentWidth = WindowWidth;
	mAspectRatio = static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_CULL_FACE);

	InitGlobalUBO();
	InitLightsSSBO();
	InitCubeVAO();
	InitNormalCubeVAO();
	InitDefaultShaders();
	InitScreenQuadVAO();
	InitSkyboxVAO();
}

void old_rebderer::InitDefaultShaders() {
	mPhongShader.Compile("Resources/Shaders/BasicShaded.shader");
	mPhongShader.Bind();
	GlobalUBO.mUnlit = false;
	GlobalUBO.mDepth = false;
	UpdateGlobalUBO();
	mOutlineShader.Compile("Resources/Shaders/Outline.shader");
	mPostProcessShader.Compile("Resources/Shaders/PostProcess.shader");
	mPostProcessShader.Bind();
	mPostProcessShader.SetUniform("u_Grayscale", false);
	mPostProcessShader.SetUniform("u_GammaCorrection", mGammaCorrection);
	mSkyboxShader.Compile("Resources/Shaders/Skybox.shader");
	mNormalsShader.Compile("Resources/Shaders/Normals.shader");
}

//void old_rebderer::ChangeViewMode(view_mode NewViewMode) {
//	if (NewViewMode != mViewMode) {
//		mViewMode = NewViewMode;
//		mPhongShader.Bind();
//		switch (mViewMode) {
//			case view_mode::lit:
//				GlobalUBO.mUnlit = false;
//				GlobalUBO.mDepth = false;
//				mDrawElementsMode = GL_TRIANGLES;
//				break;
//			case view_mode::unlit:
//				GlobalUBO.mUnlit = true;
//				GlobalUBO.mDepth = false;
//				mDrawElementsMode = GL_TRIANGLES;
//				break;
//			case view_mode::wireframe:
//				GlobalUBO.mUnlit = true;
//				GlobalUBO.mDepth = false;
//				mDrawElementsMode = GL_LINES;
//				break;
//			case view_mode::depth:
//				GlobalUBO.mUnlit = true;
//				GlobalUBO.mDepth = true;
//				mDrawElementsMode = GL_TRIANGLES;
//				break;
//		}
//		UpdateGlobalUBO();
//	}
//}

void old_rebderer::UIViewModeControl() {
//	constexpr array<view_mode, 4> Types = {
//		view_mode::lit, view_mode::unlit, view_mode::wireframe, view_mode::depth};
//	constexpr array<const char*, 4> Names = {"Lit", "Unlit", "Wireframe", "Depth"};
//	if (ImGui::BeginCombo("View mode", Names[static_cast<u32>(mViewMode)])) {
//		for (u32 i = 0; i < Types.Size(); ++i) {
//			bool bIsSelected = mViewMode == Types[i];
//			if (ImGui::Selectable(Names[i], bIsSelected)) {
//				ChangeViewMode(Types[i]);
//			}
//		}
//		ImGui::EndCombo();
//	}
}

void old_rebderer::UIPostProcessControl() {
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

void old_rebderer::InitGlobalUBO() {
	glGenBuffers(1, &GlobalUBOId);
	glBindBuffer(GL_UNIFORM_BUFFER, GlobalUBOId);
	glBufferData(GL_UNIFORM_BUFFER, 80, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, GlobalUBOBindingPoint, GlobalUBOId);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void old_rebderer::UpdateGlobalUBO() {
	glBindBuffer(GL_UNIFORM_BUFFER, GlobalUBOId);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 80, &GlobalUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void old_rebderer::InitLightsSSBO() {
	glGenBuffers(1, &LightsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsSSBO);
	GL_CALL(glBufferData(
		GL_SHADER_STORAGE_BUFFER,
		sizeof(light) * mSceneLights.Size(),
		mSceneLights.Data(),
		GL_DYNAMIC_DRAW));
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LightsSSBOBindingPoint, LightsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void old_rebderer::UpdateLightsSSBO() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsSSBO);
	if (mSceneLights.Size() > LastUpdateSize) {
		LastUpdateSize = mSceneLights.Size();
		glBufferData(
			GL_SHADER_STORAGE_BUFFER,
			mSceneLights.Size() * sizeof(light),
			mSceneLights.Data(),
			GL_DYNAMIC_DRAW);
	} else {
		glBufferSubData(
			GL_SHADER_STORAGE_BUFFER, 0, mSceneLights.Size() * sizeof(light), mSceneLights.Data());
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
