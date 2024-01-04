#pragma once

#include "basic.h"
#include "Containers/array.h"
#include "Core/String/str.h"

struct cubemap {
	u32 mRendererId{0};

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
