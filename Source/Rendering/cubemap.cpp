//
// Created by Vegust on 29.06.2023.
//

#include "cubemap.h"

#include "glad/glad.h"
#include "stb_image.h"
#include <string>

cubemap::~cubemap() {
	if (mRendererId != 0) {
		glDeleteTextures(1, &mRendererId);
	}
}

cubemap::cubemap(cubemap&& InCubemap) noexcept {
	mRendererId = InCubemap.mRendererId;
	InCubemap.mRendererId = 0;
}

cubemap& cubemap::operator=(cubemap&& InCubemap) noexcept {
	if (mRendererId != 0) {
		glDeleteTextures(1, &mRendererId);
	}
	mRendererId = InCubemap.mRendererId;
	InCubemap.mRendererId = 0;
	return *this;
}

void cubemap::Load(const str& Directory) {
	static array<str, 6> FaceNames = {
		"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"};
	Load(Directory, FaceNames);
}

void cubemap::Load(const str& Directory, const array<str, 6>& TextureFacePaths) {
	glGenTextures(1, &mRendererId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mRendererId);

	s32 Width{0};
	s32 Height{0};
	s32 NumChannels{0};
	u8* LocalBuffer{nullptr};
	for (unsigned int i = 0; i < TextureFacePaths.Size(); i++) {
		stbi_set_flip_vertically_on_load(0);
		LocalBuffer = stbi_load(
			(Directory + "/" + TextureFacePaths[i]).Raw(), &Width, &Height, &NumChannels, 0);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGB,
			Width,
			Height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			LocalBuffer);

		stbi_image_free(LocalBuffer);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void cubemap::Bind() const {
	glActiveTexture(GL_TEXTURE0 + CubemapSlot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mRendererId);
}
