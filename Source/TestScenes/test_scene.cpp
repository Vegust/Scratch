#include "test_scene.h"
#include "core_types.h"
#include "glad/glad.h"
#include "imgui.h"

test_menu::test_menu(test_scene*& InCurrentTestSceneRef)
	: CurrentTestSceneRef{InCurrentTestSceneRef} {
	struct {
		bool operator()(const scene_data& Lhs, const scene_data& Rhs) const {
			return Lhs.mName < Rhs.mName;
		}
	} CustomLess;

	GetTests().Sort(CustomLess);
}

test_menu::~test_menu() {
}

void test_menu::OnUpdate(float DeltaTime) {
	test_scene::OnUpdate(DeltaTime);
}

void test_menu::OnRender(renderer& Renderer) {
	test_scene::OnRender(Renderer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void test_menu::OnIMGuiRender() {
	test_scene::OnIMGuiRender();
	for (auto& Test : GetTests()) {
		if (ImGui::Button(Test.mName.Raw())) {
			CurrentTestSceneRef = Test.mFunction();
		}
	}
}
