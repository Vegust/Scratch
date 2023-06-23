//
// Created by Vegust on 22.06.2023.
//

#include "core_types.h"
#include "test_clear_color.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include "imgui.h"
SCRATCH_DISABLE_WARNINGS_END()

test_clear_color::test_clear_color()
{
}

test_clear_color::~test_clear_color()
{
}

void test_clear_color::OnUpdate()
{
	test_scene::OnUpdate();
}

void test_clear_color::OnRender()
{
	test_scene::OnRender();
	glClearColor(ClearColor[0], ClearColor[1], ClearColor[2], ClearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void test_clear_color::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
	ImGui::ColorEdit4("Clear Color", ClearColor.data());
}
