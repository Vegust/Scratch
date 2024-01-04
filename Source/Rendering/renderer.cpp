#include "Rendering/renderer.h"
#include "Rendering/Backend/dynamic_rhi.h"
#include "glm/ext/matrix_clip_space.hpp"

static void SetupCubeTransforms(dyn_array<glm::mat4>& StaticCubes, dyn_array<glm::mat4>& DynamicCubes) {
	// Static
	StaticCubes.Clear();
	StaticCubes.Emplace(glm::mat4(	  // 0
		glm::vec4(20, 0, 0, 0),
		glm::vec4(0, 0.1, 0, 0),
		glm::vec4(0, 0, 30, 0),
		glm::vec4(0, -0.1, 0, 1)));
	StaticCubes.Emplace(glm::mat4(	  // 1
		glm::vec4(0.1, 0, 0, 0),
		glm::vec4(0, 10, 0, 0),
		glm::vec4(0, 0, 20, 0),
		glm::vec4(5, 4.9, 0, 1)));
	StaticCubes.Emplace(glm::mat4(	  // 2
		glm::vec4(0.1, 0, 0, 0),
		glm::vec4(0, 2, 0, 0),
		glm::vec4(0, 0, 10, 0),
		glm::vec4(-5, 0.9, 5, 1)));
	StaticCubes.Emplace(glm::mat4(	  // 3
		glm::vec4(0.1, 0, 0, 0),
		glm::vec4(0, 10, 0, 0),
		glm::vec4(0, 0, 10, 0),
		glm::vec4(-5, 4.9, -5, 1)));
	StaticCubes.Emplace(glm::mat4(	  // 4
		glm::vec4(10, 0, 0, 0),
		glm::vec4(0, 10, 0, 0),
		glm::vec4(0, 0, 0.1, 0),
		glm::vec4(0, 4.9, -10, 1)));
	StaticCubes.Emplace(glm::mat4(	  // 5
		glm::vec4(10, 0, 0, 0),
		glm::vec4(0, 0.1, 0, 0),
		glm::vec4(0, 0, 10, 0),
		glm::vec4(0, 9.9, -5, 1)));
	StaticCubes.Emplace(glm::mat4(	  // 6
		glm::vec4(10, 0, 0, 0),
		glm::vec4(0, 4, 0, 0),
		glm::vec4(0, 0, 0.1, 0),
		glm::vec4(0, 7.9, 0, 1)));
	StaticCubes.Emplace(glm::mat4(	  // 7
		glm::vec4(3, 0, 0, 0),
		glm::vec4(0, 3, 0, 0),
		glm::vec4(0, 0, 3, 0),
		glm::vec4(-3, 1.4, 3, 1)));
	StaticCubes.Emplace(glm::mat4(	  // 8
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(-3, 4, 3, 1)));
	StaticCubes.Emplace(glm::mat4(	  // 9
		glm::vec4(2, 0, 0, 0),
		glm::vec4(0, 2, 0, 0),
		glm::vec4(0, 0, 2, 0),
		glm::vec4(3, 0.9, -8, 1)));
	StaticCubes.Emplace(glm::mat4(	  // 10
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 7, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(-2, 6, -7, 1)));

	// Dynamic
	DynamicCubes.Clear();
	DynamicCubes.Emplace(
		glm::mat4(glm::vec4(2, 0, 0, 0), glm::vec4(0, 2, 0, 0), glm::vec4(0, 0, 2, 0), glm::vec4(-3, 2, 7.5, 1)));
	DynamicCubes.Emplace(
		glm::mat4(glm::vec4(2, 0, 0, 0), glm::vec4(0, 2, 0, 0), glm::vec4(0, 0, 2, 0), glm::vec4(-2, 5, -4, 1)));
}

renderer::renderer(u32 WindowWidth, u32 WindowHeight) {
	mRHI = CreateRHI(mState.mApi);
	mRHI->Init();
	mOldRenderer.Init(WindowWidth, WindowHeight);

	// from testmap, TODO refactor
	auto& SceneLights = mOldRenderer.mSceneLights;
	auto& Light = SceneLights[SceneLights.Emplace()];
	Light.mType = light_type::directional;
	Light.mAmbientStrength = 0.2f;

	mSkybox.Load({"Assets/Textures/Skybox_Mountains"});

	Light.mDirection = {0.29, -0.58, -0.53};

	auto& PointLight = SceneLights[SceneLights.Emplace()];
	PointLight.mAttenuationRadius = 30.f;
	PointLight.mPosition = {0.f, 8.f, -5.f};

	framebuffer_params Params;
	Params.mHeight = mShadowmapResolution;
	Params.mWidth = mShadowmapResolution;
	Params.mType = framebuffer_type::shadowmap;
	mDirectionalShadowmap.Reload(Params);
	Params.mType = framebuffer_type::shadowmap_omni;
	mPointShadowmap.Reload(Params);

	mDirectionalShadowmapShader.Compile("Assets/Shaders/DirectionalShadowMap.shader");
	mPointShadowmapShader.Compile("Assets/Shaders/PointShadowMap.shader");

	mCubeMaterial.InitTextures(
		"Assets/Textures/Bricks/brickwall.jpg",
		"Assets/Textures/DefaultSpecular.jpg",
		"",
		"Assets/Textures/Bricks/brickwall_normal.jpg");

	Params.mType = framebuffer_type::scene;
	Params.mWidth = (s32) mOldRenderer.mCurrentWidth;
	Params.mHeight = (s32) mOldRenderer.mCurrentHeight;
	mSceneFramebuffer.Reload(Params);

	mOldRenderer.mCamera.mPosition = vec3{0.f, 1.f, 20.f};

	SetupCubeTransforms(mStaticCubes, mDynamicCubes);
}

renderer::~renderer() {
}

render_state renderer::HandleMessages(const dyn_array<app_message>& Messages) {
	dyn_array<app_message> OutMessages;
	for (const auto& Message : Messages) {
		switch (Message.Type) {
			case app_message_type::render_resize:
				mOldRenderer.OnScreenSizeChanged(Message.RenderResize.NewWidth, Message.RenderResize.NewHeight);
				mSceneFramebuffer.mParams.mHeight = (s32) Message.RenderResize.NewHeight;
				mSceneFramebuffer.mParams.mWidth = (s32) Message.RenderResize.NewWidth;
				mSceneFramebuffer.Reload(mSceneFramebuffer.mParams);
				break;
			default:
				// there should be no unhandled messages after renderer
				CHECK(false);
				break;
		}
	}
	return mState;
}

void renderer::RenderViews(const dyn_array<view>& Views) {
	// TODO this is game update
	input Input{};
	const float Speed = /*DeltaTime * */ mOldRenderer.mCamera.mMovementSpeed / 60.f;
	if (Input.Pressed(input_key::keyboard_w)) {
		mOldRenderer.mCamera.mPosition += Speed * mOldRenderer.mCamera.mDirection;
	}
	if (Input.Pressed(input_key::keyboard_s)) {
		mOldRenderer.mCamera.mPosition -= Speed * mOldRenderer.mCamera.mDirection;
	}
	if (Input.Pressed(input_key::keyboard_a)) {
		mOldRenderer.mCamera.mPosition -=
			glm::normalize(glm::cross(mOldRenderer.mCamera.mDirection, mOldRenderer.mCamera.mUpVector)) * Speed;
	}
	if (Input.Pressed(input_key::keyboard_d)) {
		mOldRenderer.mCamera.mPosition +=
			glm::normalize(glm::cross(mOldRenderer.mCamera.mDirection, mOldRenderer.mCamera.mUpVector)) * Speed;
	}
	if (Input.MouseMovedThisFrame()) {
		mOldRenderer.mCamera.mYaw += static_cast<float>(Input.GetMousePosDelta().x * mOldRenderer.mCamera.mSensitivity);
		mOldRenderer.mCamera.mPitch +=
			static_cast<float>(-Input.GetMousePosDelta().y * mOldRenderer.mCamera.mSensitivity);

		if (mOldRenderer.mCamera.mPitch > 89.0f) {
			mOldRenderer.mCamera.mPitch = 89.0f;
		}
		if (mOldRenderer.mCamera.mPitch < -89.0f) {
			mOldRenderer.mCamera.mPitch = -89.0f;
		}

		glm::vec3 NewDirection{};
		NewDirection.x = cos(glm::radians(mOldRenderer.mCamera.mYaw)) * cos(glm::radians(mOldRenderer.mCamera.mPitch));
		NewDirection.y = sin(glm::radians(mOldRenderer.mCamera.mPitch));
		NewDirection.z = sin(glm::radians(mOldRenderer.mCamera.mYaw)) * cos(glm::radians(mOldRenderer.mCamera.mPitch));
		mOldRenderer.mCamera.mDirection = glm::normalize(NewDirection);
	}
	if (Input.MouseScrolledThisFrame()) {
		mOldRenderer.mCamera.mFoV -= static_cast<float>(Input.GetScroll().y) * 5.f;
		if (mOldRenderer.mCamera.mFoV < 1.0f) {
			mOldRenderer.mCamera.mFoV = 1.0f;
		}
		if (mOldRenderer.mCamera.mFoV > 359.0f) {
			mOldRenderer.mCamera.mFoV = 359.0f;
		}
	}
	using namespace glm;
	mAccTime += /*DeltaTime * */ mUpdateSpeed / 60.f;
	float Rotation = 0.5 * glm::pi<float>() * mAccTime;
	float Translation = sin(mAccTime) * 2.f;
	mDynamicCubes[0] = scale(
		rotate(translate(mat4(1.f), vec3(-3, 2 + Translation, 7.5)), Rotation, vec3(0.5f, 1.f, 0.f)),
		vec3(2.f, 2.f, 2.f));
	mDynamicCubes[1] = scale(
		rotate(translate(mat4(1.f), vec3(-2, 5 + Translation, -4)), Rotation, vec3(0.5f, 1.f, 0.f)),
		vec3(2.f, 2.f, 2.f));
	// ---------- end of game update -----------------

	glClearColor(0.f, 1.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	using namespace glm;

	// Directional light transforms
	constexpr float NearPlane = 1.f;
	constexpr float FarPlane = 70.f;
	constexpr float SideDistance = 15.f;
	auto& Light = mOldRenderer.mSceneLights[0];
	mat4 LightProjection = ortho(-SideDistance, SideDistance, -SideDistance, SideDistance, NearPlane, FarPlane);
	// Directional light position is relative to player
	vec3 LightDirection = Light.mDirection;
	vec3 LightPosition =
		mOldRenderer.mCamera.mPosition +
		normalize(vec3(mOldRenderer.mCamera.mDirection.x, 0.f, mOldRenderer.mCamera.mDirection.z)) * SideDistance +
		(-Light.mDirection * 35.f);
	mat4 LightView = lookAt(LightPosition, LightPosition + LightDirection, vec3(0.f, 1.f, 0.f));
	mat4 LightProjectionView = LightProjection * LightView;
	Light.mShadowMatrix = LightProjectionView;

	// Point Light transforms
	const float Aspect = static_cast<float>(mDirectionalShadowmap.mParams.mWidth) /
						 static_cast<float>(mDirectionalShadowmap.mParams.mHeight);
	constexpr float Near = 0.1f;
	constexpr float Far = 25.0f;
	const auto& PointLight = mOldRenderer.mSceneLights[1];
	mat4 PointLightProjection = perspective(glm::radians(90.0f), Aspect, Near, PointLight.mAttenuationRadius);
	static const array<mat4, 6> PointLightViews{
		PointLightProjection * lookAt(vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0)),
		PointLightProjection * lookAt(vec3(0.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0)),
		PointLightProjection * lookAt(vec3(0.0, 0.0, 0.0), vec3(0.0f, 1.0, 0.0), vec3(0.0, 0.0, 1.0)),
		PointLightProjection * lookAt(vec3(0.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, -1.0)),
		PointLightProjection * lookAt(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.0, -1.0, 0.0)),
		PointLightProjection * lookAt(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, -1.0, 0.0))};

	// Directional shadow map pass
	{
		glViewport(0, 0, mDirectionalShadowmap.mParams.mWidth, mDirectionalShadowmap.mParams.mHeight);
		mDirectionalShadowmap.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		mOldRenderer.SetActiveShader(&mDirectionalShadowmapShader);
		mDirectionalShadowmapShader.Bind();
		mDirectionalShadowmapShader.SetUniform("u_ShadowMatrix", Light.mShadowMatrix);

		mOldRenderer.DrawCubes(mCubeMaterial, mStaticCubes);
		mOldRenderer.DrawCubes(mCubeMaterial, mDynamicCubes);

		mOldRenderer.SetActiveShader(&mOldRenderer.mPhongShader);
		framebuffer::SetDefault();
		glViewport(0, 0, mOldRenderer.mCurrentWidth, mOldRenderer.mCurrentHeight);
	}

	// Omnidirectional shadow map pass
	{
		glViewport(0, 0, mPointShadowmap.mParams.mWidth, mPointShadowmap.mParams.mHeight);
		mPointShadowmap.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		mOldRenderer.SetActiveShader(&mPointShadowmapShader);
		mPointShadowmapShader.Bind();
		mPointShadowmapShader.SetUniform("u_LightPos", PointLight.mPosition);
		mPointShadowmapShader.SetUniform("u_Attenuation", PointLight.mAttenuationRadius);
		mPointShadowmapShader.SetUniform("u_PointLightViews", PointLightViews);

		mOldRenderer.DrawCubes(mCubeMaterial, mStaticCubes);
		mOldRenderer.DrawCubes(mCubeMaterial, mDynamicCubes);

		mOldRenderer.SetActiveShader(&mOldRenderer.mPhongShader);
		framebuffer::SetDefault();
		glViewport(0, 0, mOldRenderer.mCurrentWidth, mOldRenderer.mCurrentHeight);
	}

	if (mDrawShadowmap) {
		mOldRenderer.DrawFrameBuffer(mDirectionalShadowmap, true);
		return;
	} else {
		mSceneFramebuffer.Bind();
		glClearColor(0.8f, 0.8f, 1.0f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0 + DIR_SHADOWMAP_TEXTURE_SLOT);
		glBindTexture(GL_TEXTURE_2D, mDirectionalShadowmap.mDepthStencilTextureId);
		glActiveTexture(GL_TEXTURE0 + OMNI_SHADOWMAP_TEXTURE_SLOT);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mPointShadowmap.mDepthStencilTextureId);

		mOldRenderer.mActiveShader->Bind();

		mOldRenderer.DrawCubes(mCubeMaterial, mStaticCubes);
		mOldRenderer.DrawCubes(mCubeMaterial, mDynamicCubes);
		mOldRenderer.DrawSkybox(mSkybox);

		framebuffer::SetDefault();

		mOldRenderer.DrawFrameBuffer(mSceneFramebuffer);
	}
}

void renderer::RenderUI(const ui_data& UIData) {
}
