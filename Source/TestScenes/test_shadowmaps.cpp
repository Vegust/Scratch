#include "test_shadowmaps.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "Rendering/bind_constants.h"

REGISTER_TEST_SCENE(test_shadowmaps, "09 Shadowmaps")

test_shadowmaps::test_shadowmaps() {
	auto& SceneLights = renderer::Get().mSceneLights;
	auto& Light = SceneLights[SceneLights.Emplace()];
	Light.mType = light_type::directional;
	Light.mAmbientStrength = 0.2f;

	mSkybox.Load({"Resources/Textures/Skybox_Mountains"});

	Light.mDirection = {0.29, -0.58, -0.53};

	auto& PointLight = SceneLights[SceneLights.Emplace()];
	PointLight.mAttenuationRadius = 30.f;
	PointLight.mPosition = {0.f, 8.f, -5.f};

	framebuffer_params Params;
	Params.mType = framebuffer_type::shadowmap;
	mDirectionalShadowmap.Reload(Params);
	Params.mType = framebuffer_type::shadowmap_omni;
	mPointShadowmap.Reload(Params);

	mDirectionalShadowmapShader.Compile("Resources/Shaders/DirectionalShadowMap.shader");
	mPointShadowmapShader.Compile("Resources/Shaders/PointShadowMap.shader");

	mCubeMaterial.InitTextures(
		"Resources/Textures/Bricks/brickwall.jpg",
		"Resources/Textures/DefaultSpecular.jpg",
		"",
		"Resources/Textures/Bricks/brickwall_normal.jpg");

	mSceneFramebuffer.Reload();

	mCamera = std::make_shared<camera>();
	mCamera->Position = glm::vec3{0.f, 1.f, 10.f};
	renderer::Get().mCustomCamera = mCamera;

	SetupCubeTransforms();
}

void test_shadowmaps::OnUpdate(float DeltaTime) {
	UpdateDynamicCubeTransforms(DeltaTime);
}

void test_shadowmaps::OnRender(renderer& Renderer) {
	glClearColor(0.f, 1.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	using namespace glm;

	// Directional light transforms
	constexpr float NearPlane = 1.f;
	constexpr float FarPlane = 70.f;
	constexpr float SideDistance = 15.f;
	auto& Light = renderer::Get().mSceneLights[0];
	mat4 LightProjection =
		ortho(-SideDistance, SideDistance, -SideDistance, SideDistance, NearPlane, FarPlane);
	// Directional light position is relative to player
	vec3 LightDirection = Light.mDirection;
	vec3 LightPosition =
		mCamera->Position +
		normalize(vec3(mCamera->Direction.x, 0.f, mCamera->Direction.z)) * SideDistance +
		(-Light.mDirection * 35.f);
	mat4 LightView = lookAt(LightPosition, LightPosition + LightDirection, vec3(0.f, 1.f, 0.f));
	mat4 LightProjectionView = LightProjection * LightView;
	Light.mShadowMatrix = LightProjectionView;

	// Point Light transforms
	const float Aspect = static_cast<float>(mDirectionalShadowmap.mParams.mWidth) /
						 static_cast<float>(mDirectionalShadowmap.mParams.mHeight);
	constexpr float Near = 0.1f;
	constexpr float Far = 25.0f;
	const auto& PointLight = renderer::Get().mSceneLights[1];
	mat4 PointLightProjection =
		perspective(glm::radians(90.0f), Aspect, Near, PointLight.mAttenuationRadius);
	static const array<mat4, 6> PointLightViews{
		PointLightProjection *
			lookAt(vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0)),
		PointLightProjection *
			lookAt(vec3(0.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0)),
		PointLightProjection *
			lookAt(vec3(0.0, 0.0, 0.0), vec3(0.0f, 1.0, 0.0), vec3(0.0, 0.0, 1.0)),
		PointLightProjection *
			lookAt(vec3(0.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, -1.0)),
		PointLightProjection *
			lookAt(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.0, -1.0, 0.0)),
		PointLightProjection *
			lookAt(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, -1.0, 0.0))};

	// Directional shadow map pass
	{
		glViewport(
			0, 0, mDirectionalShadowmap.mParams.mWidth, mDirectionalShadowmap.mParams.mHeight);
		mDirectionalShadowmap.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		Renderer.SetActiveShader(&mDirectionalShadowmapShader);
		mDirectionalShadowmapShader.Bind();
		mDirectionalShadowmapShader.SetUniform("u_ShadowMatrix", Light.mShadowMatrix);

		Renderer.DrawCubes(mCubeMaterial, mStaticCubes);
		Renderer.DrawCubes(mCubeMaterial, mDynamicCubes);

		Renderer.SetActiveShader(&Renderer.mPhongShader);
		framebuffer::SetDefault();
		glViewport(0, 0, Renderer.mCurrentWidth, Renderer.mCurrentHeight);
	}

	// Omnidirectional shadow map pass
	{
		glViewport(0, 0, mPointShadowmap.mParams.mWidth, mPointShadowmap.mParams.mHeight);
		mPointShadowmap.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		Renderer.SetActiveShader(&mPointShadowmapShader);
		mPointShadowmapShader.Bind();
		mPointShadowmapShader.SetUniform("u_LightPos", PointLight.mPosition);
		mPointShadowmapShader.SetUniform("u_Attenuation", PointLight.mAttenuationRadius);
		mPointShadowmapShader.SetUniform("u_PointLightViews", PointLightViews);

		Renderer.DrawCubes(mCubeMaterial, mStaticCubes);
		Renderer.DrawCubes(mCubeMaterial, mDynamicCubes);

		Renderer.SetActiveShader(&Renderer.mPhongShader);
		framebuffer::SetDefault();
		glViewport(0, 0, Renderer.mCurrentWidth, Renderer.mCurrentHeight);
	}

	if (mDrawShadowmap) {
		Renderer.DrawFrameBuffer(mDirectionalShadowmap, true);
		return;
	} else {
		mSceneFramebuffer.Bind();
		glClearColor(0.8f, 0.8f, 1.0f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0 + DIR_SHADOWMAP_TEXTURE_SLOT);
		glBindTexture(GL_TEXTURE_2D, mDirectionalShadowmap.mDepthStencilTextureId);
		glActiveTexture(GL_TEXTURE0 + OMNI_SHADOWMAP_TEXTURE_SLOT);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mPointShadowmap.mDepthStencilTextureId);

		Renderer.mActiveShader->Bind();

		Renderer.DrawCubes(mCubeMaterial, mStaticCubes);
		Renderer.DrawCubes(mCubeMaterial, mDynamicCubes);
		Renderer.DrawSkybox(mSkybox);

		framebuffer::SetDefault();

		Renderer.DrawFrameBuffer(mSceneFramebuffer);
	}
}

void test_shadowmaps::OnIMGuiRender() {
	ImGui::SliderFloat("Slomo", &mUpdateSpeed, 0.f, 2.f);
	ImGui::Checkbox("Draw Shadowmap", &mDrawShadowmap);
	if (ImGui::InputInt("Shadowmap Resolution", &mShadowmapResolution)) {
		framebuffer_params Params;
		Params.mType = framebuffer_type::shadowmap;
		Params.mWidth = mShadowmapResolution;
		Params.mHeight = mShadowmapResolution;
		mDirectionalShadowmap.Reload(Params);

		Params.mType = framebuffer_type::shadowmap_omni;
		mPointShadowmap.Reload(Params);
	}
	if (ImGui::CollapsingHeader("Cubes")) {
		ImGui::InputFloat("Mat shininess", &mCubeMaterial.mShininess);
		if (ImGui::Button("Add Cube")) {
			mStaticCubes.Emplace(glm::mat4(1.f));
		}

		if (mStaticCubes.Size() > 0) {
			ImGui::InputInt("Cube", &mCubeEditIndex);
			mCubeEditIndex = mCubeEditIndex % mStaticCubes.Size();
			ImGui::InputFloat4("Column 4", glm::value_ptr(mStaticCubes[mCubeEditIndex][3]));
			ImGui::InputFloat4("Column 3", glm::value_ptr(mStaticCubes[mCubeEditIndex][2]));
			ImGui::InputFloat4("Column 2", glm::value_ptr(mStaticCubes[mCubeEditIndex][1]));
			ImGui::InputFloat4("Column 1", glm::value_ptr(mStaticCubes[mCubeEditIndex][0]));
		}
	}
	if (ImGui::CollapsingHeader("Lights")) {
		ImGui::InputInt("Light", &mLightEditIndex);
		mLightEditIndex = mLightEditIndex % renderer::Get().mSceneLights.Size();
		renderer::Get().mSceneLights[mLightEditIndex].UIControlPanel("");
	}
}

void test_shadowmaps::OnScreenSizeChanged(int NewWidth, int NewHeight) {
	mSceneFramebuffer.Reload();
}

void test_shadowmaps::SetupCubeTransforms() {
	// Static
	mStaticCubes.Clear();
	mStaticCubes.Emplace(glm::mat4(	   // 0
		glm::vec4(20, 0, 0, 0),
		glm::vec4(0, 0.1, 0, 0),
		glm::vec4(0, 0, 30, 0),
		glm::vec4(0, -0.1, 0, 1)));
	mStaticCubes.Emplace(glm::mat4(	   // 1
		glm::vec4(0.1, 0, 0, 0),
		glm::vec4(0, 10, 0, 0),
		glm::vec4(0, 0, 20, 0),
		glm::vec4(5, 4.9, 0, 1)));
	mStaticCubes.Emplace(glm::mat4(	   // 2
		glm::vec4(0.1, 0, 0, 0),
		glm::vec4(0, 2, 0, 0),
		glm::vec4(0, 0, 10, 0),
		glm::vec4(-5, 0.9, 5, 1)));
	mStaticCubes.Emplace(glm::mat4(	   // 3
		glm::vec4(0.1, 0, 0, 0),
		glm::vec4(0, 10, 0, 0),
		glm::vec4(0, 0, 10, 0),
		glm::vec4(-5, 4.9, -5, 1)));
	mStaticCubes.Emplace(glm::mat4(	   // 4
		glm::vec4(10, 0, 0, 0),
		glm::vec4(0, 10, 0, 0),
		glm::vec4(0, 0, 0.1, 0),
		glm::vec4(0, 4.9, -10, 1)));
	mStaticCubes.Emplace(glm::mat4(	   // 5
		glm::vec4(10, 0, 0, 0),
		glm::vec4(0, 0.1, 0, 0),
		glm::vec4(0, 0, 10, 0),
		glm::vec4(0, 9.9, -5, 1)));
	mStaticCubes.Emplace(glm::mat4(	   // 6
		glm::vec4(10, 0, 0, 0),
		glm::vec4(0, 4, 0, 0),
		glm::vec4(0, 0, 0.1, 0),
		glm::vec4(0, 7.9, 0, 1)));
	mStaticCubes.Emplace(glm::mat4(	   // 7
		glm::vec4(3, 0, 0, 0),
		glm::vec4(0, 3, 0, 0),
		glm::vec4(0, 0, 3, 0),
		glm::vec4(-3, 1.4, 3, 1)));
	mStaticCubes.Emplace(glm::mat4(	   // 8
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(-3, 4, 3, 1)));
	mStaticCubes.Emplace(glm::mat4(	   // 9
		glm::vec4(2, 0, 0, 0),
		glm::vec4(0, 2, 0, 0),
		glm::vec4(0, 0, 2, 0),
		glm::vec4(3, 0.9, -8, 1)));
	mStaticCubes.Emplace(glm::mat4(	   // 10
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 7, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(-2, 6, -7, 1)));

	// Dynamic
	mDynamicCubes.Clear();
	mDynamicCubes.Emplace(glm::mat4(
		glm::vec4(2, 0, 0, 0),
		glm::vec4(0, 2, 0, 0),
		glm::vec4(0, 0, 2, 0),
		glm::vec4(-3, 2, 7.5, 1)));
	mDynamicCubes.Emplace(glm::mat4(
		glm::vec4(2, 0, 0, 0),
		glm::vec4(0, 2, 0, 0),
		glm::vec4(0, 0, 2, 0),
		glm::vec4(-2, 5, -4, 1)));
}

void test_shadowmaps::UpdateDynamicCubeTransforms(float DeltaTime) {
	using namespace glm;
	mAccTime += DeltaTime * mUpdateSpeed;
	float Rotation = 0.5 * glm::pi<float>() * mAccTime;
	float Translation = sin(mAccTime) * 2.f;
	mDynamicCubes[0] = scale(
		rotate(
			translate(mat4(1.f), vec3(-3, 2 + Translation, 7.5)), Rotation, vec3(0.5f, 1.f, 0.f)),
		vec3(2.f, 2.f, 2.f));
	mDynamicCubes[1] = scale(
		rotate(translate(mat4(1.f), vec3(-2, 5 + Translation, -4)), Rotation, vec3(0.5f, 1.f, 0.f)),
		vec3(2.f, 2.f, 2.f));
}
