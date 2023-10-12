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

void GlClearError();
bool GlLogCall(const char* FunctionName, const char* FileName, int LineNumber);

class old_rebderer {
public:
	void Init(u32 WindowWidth, u32 WindowHeight);
	void InitGlobalUBO();
	void InitLightsSSBO();
	void InitNormalCubeVAO();
	void InitScreenQuadVAO();
	void InitSkyboxVAO();
	void InitDefaultShaders();

	void OnScreenSizeChanged(u32 NewWidth, u32 NewHeight);

	void DrawCubes(const phong_material& Material, span<glm::mat4> Transforms);
	void DrawFrameBuffer(const framebuffer& Framebuffer, bool bDepth = false);
	void DrawSkybox(const cubemap& Skybox);

	float mAspectRatio = 1.f;
	u32 mCurrentHeight;
	u32 mCurrentWidth;

	camera mCamera{};

	float mFoV = 60.f;

	vertex_array mScreenQuadVAO{};
	vertex_array mNormalCubeVAO{};
	vertex_array mSkyboxVAO{};

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
	//void ChangeViewMode(view_mode NewViewMode);
	//view_mode mViewMode = view_mode::lit;
	s32 mDrawElementsMode = GL_TRIANGLES;
	void UIPostProcessControl();
	bool mGrayscale = false;
	bool mNormals = false;
	float mGammaCorrection = 2.2f;

	bool mInstanced = false;
};
