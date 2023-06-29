//
// Created by Vegust on 29.06.2023.
//

#include "framebuffer.h"

#include "glad/glad.h"
#include "renderer.h"

framebuffer::~framebuffer()
{
	Reset();
}

void framebuffer::Reset()
{
	if (RendererId != 0)
	{
		glDeleteFramebuffers(1, &RendererId);
	}
	if (ColorTextureId != 0)
	{
		glDeleteTextures(1, &ColorTextureId);
	}
	if (DepthStencilTextureId != 0)
	{
		glDeleteTextures(1, &DepthStencilTextureId);
	}
}

void framebuffer::Reload()
{
	Reset();
	
	glGenFramebuffers(1, &RendererId);
	glBindFramebuffer(GL_FRAMEBUFFER, RendererId);

	glGenTextures(1, &ColorTextureId);
	glBindTexture(GL_TEXTURE_2D, ColorTextureId);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		renderer::Get().CurrentWidth,
		renderer::Get().CurrentHeight,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTextureId, 0);

	glGenTextures(1, &DepthStencilTextureId);
	glBindTexture(GL_TEXTURE_2D, DepthStencilTextureId);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH24_STENCIL8,
		renderer::Get().CurrentWidth,
		renderer::Get().CurrentHeight,
		0,
		GL_DEPTH_STENCIL,
		GL_UNSIGNED_INT_24_8,
		NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, DepthStencilTextureId, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::exit(1);
	}
	
	SetDefault();
}

framebuffer::framebuffer(framebuffer&& InFramebuffer) noexcept
{
	RendererId = InFramebuffer.RendererId;
	ColorTextureId = InFramebuffer.ColorTextureId;
	DepthStencilTextureId = InFramebuffer.DepthStencilTextureId;
	InFramebuffer.RendererId = 0;
	InFramebuffer.ColorTextureId = 0;
	InFramebuffer.DepthStencilTextureId = 0;
}

framebuffer& framebuffer::operator=(framebuffer&& InFramebuffer) noexcept
{
	Reset();
	RendererId = InFramebuffer.RendererId;
	ColorTextureId = InFramebuffer.ColorTextureId;
	DepthStencilTextureId = InFramebuffer.DepthStencilTextureId;
	InFramebuffer.RendererId = 0;
	InFramebuffer.ColorTextureId = 0;
	InFramebuffer.DepthStencilTextureId = 0;
	return *this;
}

void framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, RendererId);
}

void framebuffer::SetDefault()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

