#pragma once

#include "Rendering/OldRender/framebuffer.h"
#include "SceneObjects/model.h"
#include "test_scene.h"

class test_model : public test_scene {
public:
	test_model();

	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender(renderer& Renderer) override;
	virtual void OnIMGuiRender() override;
	virtual void OnScreenSizeChanged(int NewWidth, int NewHeight) override;

	model mModel;
	framebuffer mSceneFramebuffer;
	glm::vec3 mModelPosition{0.f, 0.f, -3.f};

	bool bDrawOutline = false;
};
