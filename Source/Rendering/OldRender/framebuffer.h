#pragma once

#include "core_types.h"

enum class framebuffer_type { scene, shadowmap, shadowmap_omni };

struct framebuffer_params {
	framebuffer_type mType = framebuffer_type::scene;
	s32 mWidth = 2048;
	s32 mHeight = 2048;
};

struct framebuffer {
	u32 mRendererId{0};
	u32 mColorTextureId{0};
	u32 mDepthStencilTextureId{0};
	framebuffer_params mParams{};

	framebuffer() = default;
	~framebuffer();

	framebuffer(const framebuffer&) = delete;
	framebuffer& operator=(const framebuffer&) = delete;
	framebuffer(framebuffer&& InFramebuffer) noexcept;
	framebuffer& operator=(framebuffer&& InFramebuffer) noexcept;

	void Reload(const framebuffer_params& InParams);
	void Bind();
	void Reset();

	static void SetDefault();
};
