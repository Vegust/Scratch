//
// Created by Vegust on 22.06.2023.
//

#pragma once

#include "Rendering/renderer.h"

#include <functional>
#include <string_view>
#include <utility>
#include <vector>

#define REGISTER_TEST_SCENE(Class, Name)                           \
	_Pragma("clang diagnostic push");                              \
	_Pragma("clang diagnostic ignored \"-Wglobal-constructors\""); \
	static RegisterTestScene<Class> Test{Name};                    \
	_Pragma("clang diagnostic pop");

class test_scene
{
public:
	test_scene()
	{
	}

	virtual ~test_scene()
	{
	}

	virtual void OnUpdate(float DeltaTime)
	{
	}

	virtual void OnRender(renderer& Renderer)
	{
	}

	virtual void OnIMGuiRender()
	{
	}

	virtual void ProcessInput(struct GLFWwindow* Window, float DeltaTime)
	{
	}
};

class test_menu : public test_scene
{
public:
	explicit test_menu(test_scene*& InCurrentTestSceneRef);
	virtual ~test_menu() override;

	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;

	static std::vector<std::pair<std::string, std::function<test_scene*()>>>& GetTests()
	{
		[[clang::no_destroy]] static std::vector<std::pair<std::string, std::function<test_scene*()>>>
			Tests{};
		return Tests;
	}

private:
	test_scene*& CurrentTestSceneRef;
};

template <typename T>
struct RegisterTestScene
{
	explicit RegisterTestScene(std::string_view Name)
	{
		test_menu::GetTests().push_back(
			std::make_pair(std::string{Name}, []() { return new T(); }));
	}
};
