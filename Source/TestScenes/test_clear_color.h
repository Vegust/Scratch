//
// Created by Vegust on 22.06.2023.
//

#pragma once

#include "test_scene.h"

#include <array>

class test_clear_color : public test_scene
{
public:
	test_clear_color();
	virtual ~test_clear_color() override;
	
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
private:
	std::array<float,4> ClearColor{0.2f, 0.3f, 0.8f, 1.f};
};
