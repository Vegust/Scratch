#pragma once

#include "basic.h"
#include "Core/String/str.h"
#include "Containers/hash_table.h"
#include "Core/String/atom.h"

struct texture {
	atom mName{};
	u32 mRendererId{0};
	str mPath{};

	texture() = default;
	~texture();

	texture(const texture& Texture) = delete;
	texture& operator=(const texture&) = delete;
	texture(texture&& Texture) noexcept;
	texture& operator=(texture&& Texture) noexcept;

	struct texture_record {
		u32 mResourceId;
		u32 mRefCount;
	};

	static hash_table<str, texture_record>& GetTextureCache() {
		static hash_table<str, texture_record> TextureCache{};
		return TextureCache;
	}

	void Load(const str_view Path, bool SRGB = false);
	void Bind(u32 Slot = 0) const;
};
