#include "framebuffer.h"

#include "glad/glad.h"
#include "old_rebderer.h"

framebuffer::~framebuffer() {
	Reset();
}

void framebuffer::Reset() {
	if (mRendererId != 0) {
		glDeleteFramebuffers(1, &mRendererId);
	}
	if (mColorTextureId != 0) {
		glDeleteTextures(1, &mColorTextureId);
	}
	if (mDepthStencilTextureId != 0) {
		glDeleteTextures(1, &mDepthStencilTextureId);
	}
}

void framebuffer::Reload(const framebuffer_params& Params) {
	Reset();

	mParams = Params;

	glGenFramebuffers(1, &mRendererId);
	glBindFramebuffer(GL_FRAMEBUFFER, mRendererId);

	// for shadowmaps when sampling outside texture
	constexpr float BorderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};

	switch (Params.mType) {
		case framebuffer_type::scene:
			glGenTextures(1, &mColorTextureId);
			glBindTexture(GL_TEXTURE_2D, mColorTextureId);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGB,
				Params.mWidth,
				Params.mHeight,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTextureId, 0);

			glGenTextures(1, &mDepthStencilTextureId);
			glBindTexture(GL_TEXTURE_2D, mDepthStencilTextureId);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_DEPTH24_STENCIL8,
				Params.mWidth,
				Params.mHeight,
				0,
				GL_DEPTH_STENCIL,
				GL_UNSIGNED_INT_24_8,
				NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_DEPTH_STENCIL_ATTACHMENT,
				GL_TEXTURE_2D,
				mDepthStencilTextureId,
				0);
			break;
		case framebuffer_type::shadowmap:
			glGenTextures(1, &mDepthStencilTextureId);
			glBindTexture(GL_TEXTURE_2D, mDepthStencilTextureId);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_DEPTH_COMPONENT,
				Params.mWidth,
				Params.mHeight,
				0,
				GL_DEPTH_COMPONENT,
				GL_FLOAT,
				NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilTextureId, 0);
			glDrawBuffer(GL_NONE);	  // no color
			glReadBuffer(GL_NONE);
			break;
		case framebuffer_type::shadowmap_omni:
			glGenTextures(1, &mDepthStencilTextureId);
			glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthStencilTextureId);
			for (unsigned int i = 0; i < 6; ++i) {
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0,
					GL_DEPTH_COMPONENT,
					Params.mWidth,
					Params.mHeight,
					0,
					GL_DEPTH_COMPONENT,
					GL_FLOAT,
					NULL);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
			glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, BorderColor);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthStencilTextureId, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			break;
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Bad framebuffer" << std::endl;
		std::exit(1);
	}

	SetDefault();
}

framebuffer::framebuffer(framebuffer&& Framebuffer) noexcept {
	mRendererId = Framebuffer.mRendererId;
	mColorTextureId = Framebuffer.mColorTextureId;
	mDepthStencilTextureId = Framebuffer.mDepthStencilTextureId;
	mParams = Framebuffer.mParams;
	Framebuffer.mParams = {};
	Framebuffer.mRendererId = 0;
	Framebuffer.mColorTextureId = 0;
	Framebuffer.mDepthStencilTextureId = 0;
}

framebuffer& framebuffer::operator=(framebuffer&& Framebuffer) noexcept {
	Reset();
	mRendererId = Framebuffer.mRendererId;
	mColorTextureId = Framebuffer.mColorTextureId;
	mDepthStencilTextureId = Framebuffer.mDepthStencilTextureId;
	mParams = Framebuffer.mParams;
	Framebuffer.mParams = {};
	Framebuffer.mRendererId = 0;
	Framebuffer.mColorTextureId = 0;
	Framebuffer.mDepthStencilTextureId = 0;
	return *this;
}

void framebuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, mRendererId);
}

void framebuffer::SetDefault() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
