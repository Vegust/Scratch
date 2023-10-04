#include "test_cube.h"

#include "core_types.h"

#include <vector>

#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

#include "Rendering/OldRender/renderer.h"
#include "Rendering/OldRender/texture.h"
#include "Rendering/OldRender/vertex_buffer_layout.h"

REGISTER_TEST_SCENE(test_cube, "04 Cubes")

test_cube::test_cube() {
	mTexture.Load("Resources/Textures/Wall.jpg");
	mTexture.Bind();

	mShader.Compile("Resources/Shaders/Basic.shader");
	mShader.Bind();
	mShader.SetUniform("u_Texture", 0);

	std::srand(static_cast<u32>(std::time(nullptr)));
	mSeed = static_cast<u32>(std::rand());
}

test_cube::~test_cube() {
}

void test_cube::OnUpdate(float DeltaTime) {
	test_scene::OnUpdate(DeltaTime);
	mCurrentRotation += glm::pi<float>() / 2.f * DeltaTime;
}

void test_cube::OnRender(renderer& Renderer) {
	test_scene::OnRender(Renderer);
	glClearColor(0.2f, 0.1f, 0.1f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer.mCameraPosition = glm::vec3{0.f, 0.f, 3.f};

	std::srand(mSeed);
	dyn_array<glm::mat4> Transforms;
	for (int i = 0; i < mNumCubes; ++i) {
		glm::vec3 RandomOffset = glm::vec3{
			(std::rand() % 2000 - 1000) / 100.f,
			(std::rand() % 2000 - 1000) / 100.f,
			(std::rand() % 2000 - 2000) / 100.f};
		glm::vec3 Position = i == 0 ? mPic1Trans : mPic1Trans + RandomOffset;
		glm::mat4 ModelTransform = glm::rotate(
			glm::rotate(
				glm::rotate(
					glm::rotate(
						glm::translate(glm::mat4{1.0f}, Position),
						mPic1Rot.x,
						glm::vec3(1.f, 0.f, 0.f)),
					mPic1Rot.y,
					glm::vec3(0.f, 1.f, 0.f)),
				mPic1Rot.z,
				glm::vec3(0.f, 0.f, 1.f)),
			mCurrentRotation + static_cast<float>(i) * glm::pi<float>() / 10.f,
			glm::vec3(0.5f, 1.f, 0.f));

		Transforms.Add(ModelTransform);
	}

	Renderer.DrawCubes(mShader, Transforms);
}

void test_cube::OnIMGuiRender() {
	test_scene::OnIMGuiRender();
	constexpr float Pi = glm::pi<float>();
	ImGui::SliderFloat3("Cubes translation", glm::value_ptr(mPic1Trans), -5.f, 5.f);
	ImGui::SliderFloat3("Cubes rotation", glm::value_ptr(mPic1Rot), -Pi, Pi);
	ImGui::SliderInt("Num Cubes", &mNumCubes, 1, 10000);
}
