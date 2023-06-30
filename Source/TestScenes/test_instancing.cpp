//
// Created by Vegust on 30.06.2023.
//

#include "test_instancing.h"

REGISTER_TEST_SCENE(test_instancing, "08 Instancing")

test_instancing::test_instancing()
{
}

void test_instancing::OnRender(renderer& Renderer)
{
	test_scene::OnRender(Renderer);
	
	glClearColor(0.1f, 0.1f, 0.05f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void test_instancing::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
}