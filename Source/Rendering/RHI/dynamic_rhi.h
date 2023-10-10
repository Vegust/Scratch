#pragma once

class dynamic_rhi {
public:
	virtual ~dynamic_rhi() = default;
	virtual void Init() = 0;
	virtual void InitUIData(const void* Data) = 0;
	virtual void RenderUI() = 0;
};