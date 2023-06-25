//
// Created by Vegust on 24.06.2023.
//

#include "test_cube.h"

#include "Rendering/vertex_array.h"
#include "core_types.h"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"

#include <array>
#include <memory>
#include <vector>

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
SCRATCH_DISABLE_WARNINGS_END()

#include "Rendering/renderer.h"
#include "Rendering/texture.h"
#include "Rendering/vertex_buffer_layout.h"

REGISTER_TEST_SCENE(test_cube, "04 Cubes")

test_cube::test_cube()
{
	texture Texture{"Resources/Textures/Wall.jpg"};
	Texture.Bind();
	Shader = std::make_unique<shader>("Resources/Shaders/Basic.shader");
	Shader->Bind();
	Shader->SetUniform("u_Texture", 0);
	
	std::srand(static_cast<uint32>(std::time(nullptr)));
	Seed = static_cast<uint32>(std::rand());
}

test_cube::~test_cube()
{
}

void test_cube::OnUpdate(float DeltaTime)
{
	test_scene::OnUpdate(DeltaTime);
	CurrentRotation += glm::pi<float>() / 2.f * DeltaTime;
}

void test_cube::OnRender(renderer& Renderer)
{
	test_scene::OnRender(Renderer);
	glClearColor(0.2f, 0.1f, 0.1f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer.CameraPosition = glm::vec3{0.f, 0.f, 3.f};

	std::srand(Seed);
	std::vector<glm::mat4> Transforms;
	for (int i = 0; i < NumCubes; ++i)
	{
		glm::vec3 RandomOffset = glm::vec3{
			(std::rand() % 2000 - 1000) / 100.f,
			(std::rand() % 2000 - 1000) / 100.f,
			(std::rand() % 2000 - 2000) / 100.f};
		glm::vec3 Position = i == 0 ? Pic1Trans : Pic1Trans + RandomOffset;
		glm::mat4 ModelTransform = glm::rotate(
			glm::rotate(
				glm::rotate(
					glm::rotate(
						glm::translate(glm::mat4{1.0f}, Position),
						Pic1Rot.x,
						glm::vec3(1.f, 0.f, 0.f)),
					Pic1Rot.y,
					glm::vec3(0.f, 1.f, 0.f)),
				Pic1Rot.z,
				glm::vec3(0.f, 0.f, 1.f)),
			CurrentRotation + static_cast<float>(i) * glm::pi<float>() / 10.f,
			glm::vec3(0.5f, 1.f, 0.f));

		Transforms.push_back(ModelTransform);
	}

	Renderer.DrawCubes(*Shader, Transforms);
}

void test_cube::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
	constexpr float Pi = glm::pi<float>();
	ImGui::SliderFloat3("Cubes translation", glm::value_ptr(Pic1Trans), -5.f, 5.f);
	ImGui::SliderFloat3("Cubes rotation", glm::value_ptr(Pic1Rot), -Pi, Pi);
	ImGui::SliderInt("Num Cubes", &NumCubes, 1, 10000);
}
