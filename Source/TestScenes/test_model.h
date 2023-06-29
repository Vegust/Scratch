//
// Created by Vegust on 28.06.2023.
//

#pragma once

#include "SceneObjects/model.h"
#include "test_scene.h"

class test_model : public test_scene
{
public:
	test_model();

	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	
	model Model;
};
