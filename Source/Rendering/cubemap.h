//
// Created by Vegust on 29.06.2023.
//

#pragma once

#include "core_types.h"

#include <vector>
#include <string_view>

struct cubemap
{
	uint32 RendererId{0};
	constexpr static int32 CubemapSlot{15};
	
	cubemap() = default;
	~cubemap();
	
	cubemap(const cubemap&) = delete;
	cubemap& operator=(const cubemap&) = delete;
	cubemap(cubemap&& InCubemap) noexcept;
	cubemap& operator=(cubemap&& InCubemap) noexcept;
	
	void Load(const std::string_view& Directory, const std::vector<std::string_view>& TextureFacePaths = {});
	void Bind() const;
};
