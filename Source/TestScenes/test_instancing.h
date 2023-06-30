//
// Created by Vegust on 30.06.2023.
//

#pragma once

#include "Rendering/cubemap.h"
#include "test_scene.h"

class test_instancing : public test_scene
{
public:
	test_instancing();
	
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
};

