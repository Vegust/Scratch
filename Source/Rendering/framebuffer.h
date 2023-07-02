//
// Created by Vegust on 29.06.2023.
//

#pragma once

#include "core_types.h"

enum class framebuffer_type
{
	scene,
	shadowmap,
	shadowmap_omni
};

struct framebuffer_params
{
	framebuffer_type Type = framebuffer_type::scene;
	int32 Width = 2048;
	int32 Height = 2048;
};

struct framebuffer
{
	uint32 RendererId{0};
	uint32 ColorTextureId{0};
	uint32 DepthStencilTextureId{0};
	framebuffer_params Params{};
	
	
	framebuffer() = default;
	~framebuffer();
	
	framebuffer(const framebuffer&) = delete;
	framebuffer& operator=(const framebuffer&) = delete;
	framebuffer(framebuffer&& InFramebuffer) noexcept;
	framebuffer& operator=(framebuffer&& InFramebuffer) noexcept;
	
	void Reload(const framebuffer_params& InParams = {});
	void Bind();
	void Reset();
	
	static void SetDefault();
};
