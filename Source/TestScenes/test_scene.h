#pragma once

#include "Rendering/OldRender/renderer.h"

#include <functional>

#define REGISTER_TEST_SCENE(Class, Name) static RegisterTestScene<Class> Test{Name};

class test_scene {
public:
	test_scene() {
	}

	virtual ~test_scene() {
	}

	virtual void OnUpdate(float DeltaTime) {
	}

	virtual void OnRender(renderer& Renderer) {
	}

	virtual void OnIMGuiRender() {
	}

	virtual void OnScreenSizeChanged(int NewWidth, int NewHeight) {
	}

	virtual void ProcessInput(struct GLFWwindow* Window, float DeltaTime) {
	}
};

class test_menu : public test_scene {
public:
	explicit test_menu(test_scene*& InCurrentTestSceneRef);
	virtual ~test_menu() override;

	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;

	struct scene_data {
		str mName{};
		std::function<test_scene*()> mFunction{};
	};

	static dyn_array<scene_data>& GetTests() {
		[[clang::no_destroy]] static dyn_array<scene_data> Tests{};
		return Tests;
	}

private:
	test_scene*& CurrentTestSceneRef;
};

template <typename T>
struct RegisterTestScene {
	explicit RegisterTestScene(const str& Name) {
		test_menu::GetTests().Emplace(test_menu::scene_data{Name, []() { return new T(); }});
	}
};
