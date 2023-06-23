//
// Created by Vegust on 22.06.2023.
//

#pragma once

#include <functional>
#include <string_view>
#include <utility>
#include <vector>

class test_scene
{
public:
	test_scene()
	{
	}

	virtual ~test_scene()
	{
	}

	virtual void OnUpdate()
	{
	}

	virtual void OnRender()
	{
	}

	virtual void OnIMGuiRender()
	{
	}
};

class test_menu : public test_scene
{
public:
	explicit test_menu(test_scene*& InCurrentTestSceneRef);
	virtual ~test_menu() override;

	virtual void OnUpdate() override;
	virtual void OnRender() override;
	virtual void OnIMGuiRender() override;
	
	template<typename T>
	void RegisterTest(std::string_view Name)
	{
		Tests.push_back(std::make_pair(std::string{Name},[](){ return new T(); }));
	}

private:
	test_scene*& CurrentTestSceneRef;
	std::vector<std::pair<std::string, std::function<test_scene*()>>> Tests{};
};
