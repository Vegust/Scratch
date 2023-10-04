#include "test_basic_light.h"

#include "core_types.h"
#include "glm/matrix.hpp"

#include "glad/glad.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

#include "Rendering/OldRender/renderer.h"
#include "Rendering/OldRender/vertex_buffer_layout.h"

REGISTER_TEST_SCENE(test_basic_light, "05 Basic (Phong?) Light")

test_basic_light::test_basic_light() {
	mShader.Compile("Resources/Shaders/BasicShaded.shader");
	mLightShader.Compile("Resources/Shaders/BasicLight.shader");

	mCubeMaterial.InitTextures(
		"Resources/Textures/Box/BoxDiffuse.png", 0, "Resources/Textures/Box/BoxSpecular.png", 1);
	//"Resources/Textures/OpenGL_Logo.png",
	// 2);

	mCamera = std::make_shared<camera>();
	mCamera->Position = glm::vec3{0.f, 0.f, 1.5f};
	renderer::Get().mCustomCamera = mCamera;
}

void test_basic_light::OnUpdate(float DeltaTime) {
	test_scene::OnUpdate(DeltaTime);
	if (mRotating) {
		mCurrentRotation += glm::pi<float>() / 2.f * DeltaTime;
	}
}

void test_basic_light::OnRender(renderer& Renderer) {
	test_scene::OnRender(Renderer);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const glm::mat4 View = mCamera->GetViewTransform();

	std::srand(0);
	array<glm::mat4,10> Transforms;
	for (int i = 0; i < 10; ++i) {
		glm::vec3 RandomOffset = glm::vec3{
			(std::rand() % 2000 - 1000) / 200.f,
			(std::rand() % 2000 - 1000) / 200.f,
			(std::rand() % 2000 - 2000) / 200.f};
		glm::vec3 Position = i == 0 ? mCubePosition : mCubePosition + RandomOffset;
		glm::mat4 ModelTransform = glm::rotate(
			glm::translate(glm::mat4{1.0f}, Position),
			mCurrentRotation + static_cast<float>(i) * glm::pi<float>() / 10.f,
			glm::vec3(0.5f, 1.f, 0.f));
		Transforms[i] = ModelTransform;
	}

	mShader.Bind();
	mShader.SetUniform("u_Material", mCubeMaterial);
	mShader.SetUniform("u_Lights", "u_NumLights", mLights, View);
	Renderer.DrawNormalCubes(mShader, Transforms);

	dyn_array<glm::mat4> LightTransforms;
	for (const auto& Light : mLights) {
		glm::mat4 LightTransform = glm::scale(
			glm::rotate(
				glm::translate(glm::mat4{1.0f}, Light.mPosition),
				mCurrentRotation,
				glm::vec3(-0.5f, 1.f, 0.f)),
			glm::vec3(0.2f, 0.2f, 0.2f));
		LightTransforms.Add(LightTransform);
		mLightShader.Bind();
		// This won't work properly for light cubes color right now but i don't care tbh
		mLightShader.SetUniform("u_Light", Light, View);
	}

	Renderer.DrawNormalCubes(mLightShader, LightTransforms);
}

void test_basic_light::OnIMGuiRender() {
	test_scene::OnIMGuiRender();
	ImGui::SliderFloat3("Cube Position", glm::value_ptr(mCubePosition), -5.f, 5.f);
	ImGui::SliderFloat("Cube Shininess", &mCubeMaterial.mShininess, 2.f, 256.f);
	for (u32 i = 0; i < mLights.Size(); ++i) {
		mLights[i].UIControlPanel(str(i));
	}
	if (mLights.Size() < 100 && ImGui::Button("Add Light")) {
		mLights.Emplace();
	}
	if (mLights.Size() > 0 && ImGui::Button("Remove Light")) {
		mLights.RemoveAt(mLights.Size()-1);
	}

	ImGui::Checkbox("Rotating", &mRotating);
}
