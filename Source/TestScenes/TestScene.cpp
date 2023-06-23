//
// Created by Vegust on 22.06.2023.
//

#include "core_types.h"
#include "test_scene.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include "imgui.h"
SCRATCH_DISABLE_WARNINGS_END()

test_menu::test_menu(test_scene*& InCurrentTestSceneRef)
	: CurrentTestSceneRef{InCurrentTestSceneRef}
{
}

test_menu::~test_menu()
{
}

void test_menu::OnUpdate()
{
	test_scene::OnUpdate();
}

void test_menu::OnRender()
{
	test_scene::OnRender();
	glClear(GL_COLOR_BUFFER_BIT);
}

void test_menu::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
	for (auto& Test : Tests)
	{
		if (ImGui::Button(Test.first.c_str()))
		{
			CurrentTestSceneRef = Test.second();
		}
	}
}
