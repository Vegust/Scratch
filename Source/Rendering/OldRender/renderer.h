#pragma once

#include "SceneObjects/camera.h"
#include "core_types.h"
#include "vertex_array.h"
#include "cubemap.h"
#include "framebuffer.h"
#include "glad/glad.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/glm.hpp"
#include "shader.h"
#include "vertex_buffer.h"

#define GL_CALL(x)  \
	GlClearError(); \
	x;              \
	GlLogCall(#x, __FILE__, __LINE__);

class shader;

enum class view_mode : u8 { lit = 0, unlit = 1, wireframe = 2, depth = 3 };

void GlClearError();
bool GlLogCall(const char* FunctionName, const char* FileName, int LineNumber);

class renderer {
public:
	static void Clear();
	static void Draw(
		const vertex_array& VertexArray,
		const element_buffer& IndexBuffer,
		const shader& Shader);

	void Init(u32 WindowHeight, u32 WindowWidth);
	void InitGlobalUBO();
	void InitLightsSSBO();
	void InitCubeVAO();
	void InitNormalCubeVAO();
	void InitScreenQuadVAO();
	void InitSkyboxVAO();
	void InitDefaultShaders();

	void Draw(
		const vertex_array& VertexArray,
		const element_buffer& IndexBuffer,
		const shader& Shader,
		glm::mat4 Transform) const;
	void DrawCubes(const shader& Shader, span<glm::mat4> Transforms) const;
	void DrawCubes(const phong_material& Material, span<glm::mat4> Transforms);
	void DrawNormalCubes(const shader& Shader, span<glm::mat4> Transforms) const;
	void Draw2(
		const vertex_array& VertexArray,
		const element_buffer& ElementBuffer,
		const phong_material& Material,
		const glm::mat4& Transform) const;
	void DrawFrameBuffer(const framebuffer& Framebuffer, bool bDepth = false);
	void DrawSkybox(const cubemap& Skybox);

	void ResetCamera() {
		mCameraPosition = glm::vec3{0.f, 0.f, 0.f};
		mCameraDirection = glm::vec3{0.f, 0.f, -1.f};
		mCameraUpVector = glm::vec3{0.f, 1.f, 0.f};
	}

	glm::mat4 CalcMVPForTransform(const glm::mat4& Transform) const;

	float mAspectRatio = 1.f;
	u32 mCurrentHeight;
	u32 mCurrentWidth;

	std::weak_ptr<camera> mCustomCamera{};

	float mFoV = 60.f;
	glm::vec3 mCameraPosition = glm::vec3{0.f, 0.f, 0.f};
	glm::vec3 mCameraDirection = glm::vec3{0.f, 0.f, -1.f};
	glm::vec3 mCameraUpVector = glm::vec3{0.f, 1.f, 0.f};

	vertex_array mScreenQuadVAO{};
	vertex_buffer mScreenQuadBVO{};

	vertex_array mCubeVAO{};
	vertex_buffer mCubeVBO{};

	vertex_array mNormalCubeVAO{};
	vertex_buffer mNormalCubeVBO{};

	vertex_array mSkyboxVAO{};
	vertex_buffer mSkyboxVBO{};
	s32 mSkyboxTextureSlot = cubemap::CubemapSlot;

	shader mPostProcessShader{};

	shader mSkyboxShader{};
	shader mPhongShader{};
	shader mOutlineShader{};
	shader mNormalsShader{};
	shader* mActiveShader = &mPhongShader;

	u32 GlobalUBOBindingPoint = 0;
	u32 GlobalUBOId = 0;

	struct global_ubo {
		glm::mat4 mProjection;
		bool mUnlit;
		u16 : 16;
		bool mDepth;
		u64 : 48;
	} GlobalUBO;

	void UpdateGlobalUBO();

	u32 LightsSSBOBindingPoint = 0;
	u32 LightsSSBO{0};
	u32 LastUpdateSize = 0;
	dyn_array<light> mSceneLights{};
	void UpdateLightsSSBO();

	void SetActiveShader(shader* NewActiveShader) {
		mActiveShader = NewActiveShader;
	}

	void UIRendererControls() {
		UIViewModeControl();
		UIPostProcessControl();
	}

	void UIViewModeControl();
	void ChangeViewMode(view_mode NewViewMode);
	view_mode mViewMode = view_mode::lit;
	s32 mDrawElementsMode = GL_TRIANGLES;
	void UIPostProcessControl();
	bool mGrayscale = false;
	bool mNormals = false;
	float mGammaCorrection = 2.2f;

	bool mInstanced = false;
};
