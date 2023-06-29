//
// Created by Vegust on 28.06.2023.
//

#include "test_model.h"

REGISTER_TEST_SCENE(test_model, "06 Mesh")

test_model::test_model()
{
	renderer::Get().SceneLights.emplace_back();
	Model.Load("Resources/Models/Backpack/backpack.obj");
}

void test_model::OnUpdate(float DeltaTime)
{
	test_scene::OnUpdate(DeltaTime);
}

void test_model::OnRender(renderer& Renderer)
{
	test_scene::OnRender(Renderer);
	glClearColor(0.2f, 0.f, 0.1f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Model.Draw(Renderer, glm::mat4{1.f});
}

void test_model::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
}
