//
// Created by Vegust on 29.06.2023.
//

#pragma once

#include "core_types.h"

struct cubemap
{
	uint32 RendererId{0};
	
	cubemap() = default;
	~cubemap();
	
	cubemap(const cubemap&) = delete;
	cubemap& operator=(const cubemap&) = delete;
	cubemap(cubemap&& InCubemap) noexcept;
	cubemap& operator=(cubemap&& InCubemap) noexcept;
	
	void Load();
};
