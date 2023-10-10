#pragma once

#include "core_types.h"

class dynamic_rhi {
public:
	virtual ~dynamic_rhi() = default;
	virtual void Init() = 0;
	virtual void InitUIData(const void* Data) = 0;
	virtual void RenderUI() = 0;
};

enum class rendering_api : u8;
dynamic_rhi* CreateRHI(rendering_api Api);