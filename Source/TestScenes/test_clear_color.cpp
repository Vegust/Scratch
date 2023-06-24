//
// Created by Vegust on 22.06.2023.
//

#include "core_types.h"
#include "test_clear_color.h"
#include "test_scene.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include "imgui.h"
SCRATCH_DISABLE_WARNINGS_END()

REGISTER_TEST_SCENE(test_clear_color)

test_clear_color::test_clear_color()
{
}

test_clear_color::~test_clear_color()
{
}

void test_clear_color::OnUpdate(float DeltaTime)
{
	test_scene::OnUpdate(DeltaTime);
}

void test_clear_color::OnRender(const renderer& Renderer)
{
	test_scene::OnRender(Renderer);
	glClearColor(ClearColor[0], ClearColor[1], ClearColor[2], ClearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void test_clear_color::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
	ImGui::ColorEdit4("Clear Color", ClearColor.data());
}
