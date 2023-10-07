//
// Created by Vegust on 29.06.2023.
//

#pragma once

#include "core_types.h"
#include "Containers/array.h"
#include "Containers/str.h"

struct cubemap {
	u32 mRendererId{0};
	constexpr static s32 CubemapSlot{15};

	cubemap() = default;
	~cubemap();

	cubemap(const cubemap&) = delete;
	cubemap& operator=(const cubemap&) = delete;
	cubemap(cubemap&& InCubemap) noexcept;
	cubemap& operator=(cubemap&& InCubemap) noexcept;

	void Load(const str& Directory);
	void Load(const str& Directory, const array<str, 6>& TextureFacePaths);
	void Bind() const;
};