//
// Created by Vegust on 29.06.2023.
//

#pragma once

#include "core_types.h"

struct framebuffer
{
	uint32 RendererId{0};
	uint32 ColorTextureId{0};
	uint32 DepthStencilTextureId{0};
	
	framebuffer() = default;
	~framebuffer();
	
	framebuffer(const framebuffer&) = delete;
	framebuffer& operator=(const framebuffer&) = delete;
	framebuffer(framebuffer&& InFramebuffer) noexcept;
	framebuffer& operator=(framebuffer&& InFramebuffer) noexcept;
	
	void Reload();
	void Bind();
	void Reset();
	
	static void SetDefault();
};
