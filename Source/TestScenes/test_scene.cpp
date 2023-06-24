//
// Created by Vegust on 22.06.2023.
//

#include "test_scene.h"

#include "core_types.h"

#include <algorithm>

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include "imgui.h"
SCRATCH_DISABLE_WARNINGS_END()

test_menu::test_menu(test_scene*& InCurrentTestSceneRef)
	: CurrentTestSceneRef{InCurrentTestSceneRef}
{
	struct
	{
		bool operator()(
			const std::pair<std::string, std::function<test_scene*()>>& a,
			const std::pair<std::string, std::function<test_scene*()>>& b) const
		{
			return a.first < b.first;
		}
	} CustomLess;

	std::sort(GetTests().begin(), GetTests().end(), CustomLess);
}

test_menu::~test_menu()
{
}

void test_menu::OnUpdate(float DeltaTime)
{
	test_scene::OnUpdate(DeltaTime);
}

void test_menu::OnRender(renderer& Renderer)
{
	test_scene::OnRender(Renderer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void test_menu::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
	for (auto& Test : GetTests())
	{
		if (ImGui::Button(Test.first.c_str()))
		{
			CurrentTestSceneRef = Test.second();
		}
	}
}
