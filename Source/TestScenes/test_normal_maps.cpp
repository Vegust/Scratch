#include "test_normal_maps.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

REGISTER_TEST_SCENE(test_normal_maps, "10 Normals")

test_normal_maps::test_normal_maps() {
	auto& SceneLights = renderer::Get().mSceneLights;
	auto& Light = SceneLights[SceneLights.Emplace()];
	Light.mPosition = {0.f, 0.f, 2.f};
	Light.mSpecular = {0.5f, 0.5f, 0.5f};

	mLightShader.Compile("Resources/Shaders/BasicLight.shader");

	mCubeMaterial.InitTextures(
		"Resources/Textures/Bricks/brickwall.jpg",
		0,
		"Resources/Textures/DefaultSpecular.jpg",
		1,
		"",
		2,
		"Resources/Textures/Bricks/brickwall_normal.jpg",
		3);
	mCubeMaterial.mShininess = 8.f;
	mSceneFramebuffer.Reload();

	mCamera = std::make_shared<camera>();
	mCamera->Position = glm::vec3{0.f, 0.5f, 2.f};
	renderer::Get().mCustomCamera = mCamera;

	mStaticCubes.Add(glm::mat4(1.f));
}

void test_normal_maps::OnUpdate(float DeltaTime) {
	mAccTime += DeltaTime * mUpdateSpeed;
	auto& Light = renderer::Get().mSceneLights[0];
	const float Rotation = glm::pi<float>() * mAccTime;
	Light.mPosition = glm::translate(
						  glm::rotate(glm::mat4(1.f), Rotation, glm::vec3(0.5, 0.5, 0.1)),
						  glm::vec3(0.f, 0.f, 2.f)) *
					  glm::vec4(0.f, 0.f, 0.f, 1.f);
}

void test_normal_maps::OnRender(renderer& Renderer) {
	glClearColor(0.f, 1.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	mSceneFramebuffer.Bind();
	glClearColor(0.03f, 0.03f, 0.03f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Renderer.DrawCubes(mCubeMaterial, mStaticCubes);

	// Light
	const glm::mat4 View = mCamera->GetViewTransform();
	dyn_array<glm::mat4> LightTransforms;
	const float Rotation = glm::pi<float>() * mAccTime;
	glm::mat4 LightTransform = glm::scale(
		glm::translate(glm::mat4{1.0f}, renderer::Get().mSceneLights[0].mPosition),
		glm::vec3(0.1f, 0.1f, 0.1f));
	LightTransforms.Emplace(LightTransform);
	mLightShader.Bind();
	mLightShader.SetUniform("u_Light", renderer::Get().mSceneLights[0], View);
	Renderer.DrawNormalCubes(mLightShader, LightTransforms);

	framebuffer::SetDefault();

	Renderer.DrawFrameBuffer(mSceneFramebuffer);
}

void test_normal_maps::OnIMGuiRender() {
	ImGui::SliderFloat("Slomo", &mUpdateSpeed, 0.f, 2.f);
	if (ImGui::CollapsingHeader("Light")) {
		renderer::Get().mSceneLights[0].UIControlPanel("");
	}
}

void test_normal_maps::OnScreenSizeChanged(int NewWidth, int NewHeight) {
	mSceneFramebuffer.Reload();
}
