#include "test_instancing.h"

#include "imgui.h"

REGISTER_TEST_SCENE(test_instancing, "08 Instancing")

test_instancing::test_instancing() {
	mSunShader.Compile("Resources/Shaders/BasicLight.shader");
	mInstancedPhongShader.Compile("Resources/Shaders/BasicShadedInstanced.shader");
	mModel.Load("Resources/Models/Rock/rock.obj");

	array<str, 6> Paths = {
		"Left_Tex.jpg",
		"Right_Tex.jpg",
		"Up_Tex.jpg",
		"Down_Tex.jpg",
		"Front_Tex.jpg",
		"Back_Tex.jpg"};
	mSkybox.Load("Resources/Textures/Skybox_Stars", Paths);

	auto& SceneLights = renderer::Get().mSceneLights;
	auto& Light = SceneLights[SceneLights.Emplace()];
	Light.mType = light_type::point;
	Light.mAttenuationRadius = 1000.f;

	mCamera = std::make_shared<camera>();
	mCamera->Position = glm::vec3{0.f, 1.f, 10.f};
	renderer::Get().mCustomCamera = mCamera;

	RecalculateAsteroidTransforms();
}

void test_instancing::OnRender(renderer& Renderer) {
	test_scene::OnRender(Renderer);

	glClearColor(0.1f, 0.1f, 0.05f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Sun
	const glm::mat4 View = mCamera->GetViewTransform();
	dyn_array<glm::mat4> LightTransforms;
	glm::mat4 LightTransform = glm::scale(
		glm::translate(glm::mat4{1.0f}, renderer::Get().mSceneLights[0].mPosition),
		glm::vec3(1.f, 1.0f, 1.0f));
	LightTransforms.Add(LightTransform);
	mSunShader.Bind();
	mSunShader.SetUniform("u_Light", renderer::Get().mSceneLights[0], View);
	Renderer.DrawNormalCubes(mSunShader, LightTransforms);

	// Asteroids
	if (mInstancing) {
		Renderer.SetActiveShader(&mInstancedPhongShader);
		Renderer.mInstanced = true;
		mModel.Draw(Renderer, glm::mat4(1.f));	  // Transforms stored in bound instanced buffer
		Renderer.mInstanced = false;
		Renderer.SetActiveShader(&Renderer.mPhongShader);
	} else {
		for (const auto& Transform : mAsteroidTransforms) {
			mModel.Draw(Renderer, Transform);
		}
	}

	// Stars
	Renderer.DrawSkybox(mSkybox);
}

void test_instancing::OnIMGuiRender() {
	test_scene::OnIMGuiRender();

	const s32 OldAsteroidCount = mAsteroidCount;
	const float OldInnerRadius = mInnerRadius;
	const float OldOuterRadius = mOuterRadius;
	const float OldVerticalSpread = mVerticalSpread;

	ImGui::Checkbox("Instancing", &mInstancing);
	ImGui::InputInt("Asteroid count", &mAsteroidCount);
	ImGui::InputFloat("Inner Radius", &mInnerRadius);
	ImGui::InputFloat("Outer Radius", &mOuterRadius);
	ImGui::InputFloat("Vertical Spread", &mVerticalSpread);

	if (OldAsteroidCount != mAsteroidCount || OldInnerRadius != mInnerRadius ||
		OldOuterRadius != mOuterRadius || OldVerticalSpread != mVerticalSpread) {
		RecalculateAsteroidTransforms();
	}

	renderer::Get().mSceneLights[0].UIControlPanel("");
}

void test_instancing::RecalculateAsteroidTransforms() {
	mAsteroidTransforms.Clear();
	mAsteroidTransforms.Reserve(mAsteroidCount);
	for (s32 i = 0; i < mAsteroidCount; ++i) {
		using namespace glm;
		const float VerticalOffset = mVerticalSpread * (((rand() % 10000) / 5000.f) - 1.f);
		const float RotationLocal = pi<float>() * 2 * ((rand() % 10000) / 10000.f);
		const float Radius =
			mInnerRadius + ((rand() % 10000) / 10000.f) * (mOuterRadius - mInnerRadius);
		const float Rotation = pi<float>() * 2 * ((rand() % 10000) / 10000.f);
		mAsteroidTransforms.Add(scale(
			rotate(
				translate(
					rotate(
						translate(mat4(1.f), vec3(0.f, VerticalOffset, 0.f)),
						Rotation,
						vec3(0.f, 1.f, 0.f)),
					vec3(Radius, 0.f, 0.f)),
				RotationLocal,
				vec3(1.f, 0.3f, 0.1f)),
			vec3(0.1f, 0.1f, 0.1f)));
	}

	// Instanced buffer
	mInstancesBuffer = {};	  // Reset
	mInstancesBuffer.SetData(
		mAsteroidTransforms.Data(), mAsteroidTransforms.Size() * sizeof(glm::mat4));
	mInstancesBuffer.Bind();
	for (auto& Mesh : mModel.mMeshes) {
		Mesh.mVertexArray.Bind();
		Mesh.mVertexArray.mInstanceCount = mAsteroidTransforms.Size();

		u64 Vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * Vec4Size, (void*) 0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * Vec4Size, (void*) (1 * Vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * Vec4Size, (void*) (2 * Vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * Vec4Size, (void*) (3 * Vec4Size));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
}
