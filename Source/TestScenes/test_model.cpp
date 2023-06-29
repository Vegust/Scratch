//
// Created by Vegust on 28.06.2023.
//

#include "test_model.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

REGISTER_TEST_SCENE(test_model, "06 Mesh")

test_model::test_model()
{
	auto& Light = renderer::Get().SceneLights.emplace_back();
	Light.Type = light_type::directional;
	Light.Ambient = glm::vec3{0.5f, 0.5f, 0.5f};
	Light.Diffuse = glm::vec3{0.5f, 0.5f, 0.5};

	SceneFramebuffer.Reload();

	Model.Load("Resources/Models/Backpack/backpack.obj");
}

void test_model::OnUpdate(float DeltaTime)
{
	test_scene::OnUpdate(DeltaTime);
}

void test_model::OnRender(renderer& Renderer)
{
	test_scene::OnRender(Renderer);
	glClearColor(0.1f, 0.2f, 0.1f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	{
		SceneFramebuffer.Bind();

		glClearColor(0.2f, 0.f, 0.1f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		Model.Draw(Renderer, glm::translate(glm::mat4{1.f}, ModelPosition));

		if (bDrawOutline)
		{
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			Renderer.SetActiveShader(&Renderer.OutlineShader);
			Model.Draw(Renderer, glm::translate(glm::mat4{1.f}, ModelPosition));
			Renderer.SetActiveShader(&Renderer.PhongShader);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);
		}

		framebuffer::SetDefault();
	}
	
	Renderer.DrawFrameBuffer(SceneFramebuffer);
}

void test_model::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();
	ImGui::Checkbox("Draw outline", &bDrawOutline);
	ImGui::SliderFloat3("Model Position", glm::value_ptr(ModelPosition), -5.f, 5.f);
	renderer::Get().SceneLights[0].UIControlPanel("");
}

void test_model::OnScreenSizeChanged(int NewWidth, int NewHeight)
{
	test_scene::OnScreenSizeChanged(NewWidth, NewHeight);
	SceneFramebuffer.Reload();
}
