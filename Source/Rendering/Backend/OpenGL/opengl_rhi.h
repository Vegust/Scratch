#pragma once

#include "Rendering/Backend/dynamic_rhi.h"

class opengl_rhi : public rhi {
public:
	void Init() override;
	void InitUIData(const void* Data) override;
	void RenderUI() override;
};