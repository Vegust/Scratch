#pragma once

#include "Rendering/RHI/dynamic_rhi.h"

class opengl_rhi : public dynamic_rhi {
public:
	~opengl_rhi() override;
	void Init() override;
	void InitUIData(const void* Data) override;
	void RenderUI() override;
};