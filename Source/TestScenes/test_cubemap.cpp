#include "test_cubemap.h"

#include "imgui.h"

REGISTER_TEST_SCENE(test_cubemap, "07 Cubemap")

test_cubemap::test_cubemap() {
	mCubeMaterial.InitTextures(
		"Resources/Textures/Box/BoxDiffuse.png", 0, "Resources/Textures/Box/BoxSpecular.png", 1);

	mSkybox.Load({"Resources/Textures/Skybox_Mountains"});

	mMirrorShader.Compile("Resources/Shaders/Mirror.shader");
	mMirrorShader.SetUniform("u_Cubemap", cubemap::CubemapSlot);

	mGlassShader.Compile("Resources/Shaders/Glass.shader");
	mGlassShader.SetUniform("u_Cubemap", cubemap::CubemapSlot);

	mCamera = std::make_shared<camera>();
	mCamera->Position = glm::vec3{0.f, 0.f, 1.5f};
	renderer::Get().mCustomCamera = mCamera;

	auto& SceneLights = renderer::Get().mSceneLights;
	light& Light = SceneLights[SceneLights.Emplace()];
	Light.mType = light_type::directional;
	Light.mAmbient = glm::vec3{0.5f, 0.5f, 0.5f};
	Light.mDiffuse = glm::vec3{0.5f, 0.5f, 0.5};
}

void test_cubemap::OnRender(renderer& Renderer) {
	test_scene::OnRender(Renderer);

	glClearColor(0.1f, 0.2f, 0.1f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	switch (mMatType) {
		case mat_default:
			break;
		case mat_mirror:
			Renderer.SetActiveShader(&mMirrorShader);
			mMirrorShader.Bind();
			mSkybox.Bind();
			mMirrorShader.SetUniform("u_Cubemap", cubemap::CubemapSlot);
			break;
		case mat_glass:
			Renderer.SetActiveShader(&mGlassShader);
			mGlassShader.Bind();
			mSkybox.Bind();
			mGlassShader.SetUniform("u_Cubemap", cubemap::CubemapSlot);
			mGlassShader.SetUniform("u_RefractiveIndex", mRefractiveIndex);
			break;
	}

	dyn_array<glm::mat4> CubeTransforms;
	CubeTransforms.Add(glm::translate(glm::mat4{1.f}, mCubePosition));
	Renderer.DrawCubes(mCubeMaterial, CubeTransforms);
	Renderer.SetActiveShader(&Renderer.mPhongShader);

	Renderer.DrawSkybox(mSkybox);
}

void test_cubemap::OnIMGuiRender() {
	test_scene::OnIMGuiRender();

	constexpr array<mat_type, 3> Types = {mat_default, mat_mirror, mat_glass};
	constexpr array<const char*, 3> TypeNames = {"default", "mirror", "glass"};

	if (ImGui::BeginCombo("Mat type", TypeNames[mMatType])) {
		for (u32 i = 0; i < Types.Size(); ++i) {
			bool bIsSelected = mMatType == Types[i];
			if (ImGui::Selectable(TypeNames[Types[i]], bIsSelected)) {
				mMatType = Types[i];
			}
		}
		ImGui::EndCombo();
	}

	if (mMatType == mat_type::mat_glass) {
		ImGui::SliderFloat("Refractive Index", &mRefractiveIndex, 0.0f, 5.0f);
	}

	renderer::Get().mSceneLights[0].UIControlPanel("");
}
