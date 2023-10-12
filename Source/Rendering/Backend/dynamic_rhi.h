#pragma once

#include "core_types.h"

class dynamic_rhi {
public:
	virtual ~dynamic_rhi() = default;
	virtual void Init() = 0;
	virtual void InitUIData(const void* Data) = 0;
	virtual void RenderUI() = 0;
};

// TODO: maybe later allow non-virtual (compile time) render backend resolution because
// why would you have 2+ different rendering backends for 1 platform in shipping build?
using rhi = dynamic_rhi;

enum class rendering_api : u8;
rhi* CreateRHI(rendering_api Api);

