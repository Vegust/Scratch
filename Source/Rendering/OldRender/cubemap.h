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

	void Load(str_view Directory);
	void Load(str_view Directory, const array<str_view, 6>& TextureFacePaths);
	void Bind() const;
};
