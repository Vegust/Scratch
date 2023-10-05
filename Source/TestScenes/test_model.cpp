#include "test_model.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

REGISTER_TEST_SCENE(test_model, "06 Mesh")

test_model::test_model() {
	auto& SceneLights = renderer::Get().mSceneLights;
	auto& Light = SceneLights[SceneLights.Emplace()];
	Light.mType = light_type::directional;

	mSceneFramebuffer.Reload();

	mModel.Load("Resources/Models/Backpack/backpack.obj");
}

void test_model::OnUpdate(float DeltaTime) {
	test_scene::OnUpdate(DeltaTime);
}

void test_model::OnRender(renderer& Renderer) {
	test_scene::OnRender(Renderer);
	glClearColor(0.1f, 0.2f, 0.1f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	{
		mSceneFramebuffer.Bind();

		glClearColor(0.2f, 0.f, 0.1f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		mModel.Draw(Renderer, glm::translate(glm::mat4{1.f}, mModelPosition));

		if (bDrawOutline) {
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			Renderer.SetActiveShader(&Renderer.mOutlineShader);
			mModel.Draw(Renderer, glm::translate(glm::mat4{1.f}, mModelPosition));
			Renderer.SetActiveShader(&Renderer.mPhongShader);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);
		}

		framebuffer::SetDefault();
	}

	Renderer.DrawFrameBuffer(mSceneFramebuffer);
}

void test_model::OnIMGuiRender() {
	test_scene::OnIMGuiRender();
	ImGui::Checkbox("Draw outline", &bDrawOutline);
	ImGui::SliderFloat3("Model Position", glm::value_ptr(mModelPosition), -5.f, 5.f);
	renderer::Get().mSceneLights[0].UIControlPanel("");
}

void test_model::OnScreenSizeChanged(int NewWidth, int NewHeight) {
	test_scene::OnScreenSizeChanged(NewWidth, NewHeight);
	mSceneFramebuffer.Reload();
}
