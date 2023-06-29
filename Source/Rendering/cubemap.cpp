//
// Created by Vegust on 29.06.2023.
//

#include "cubemap.h"

#include "glad/glad.h"
#include "stb_image.h"

cubemap::~cubemap()
{
	if (RendererId != 0)
	{
		glDeleteTextures(1, &RendererId);
	}
}

cubemap::cubemap(cubemap&& InCubemap) noexcept
{
	RendererId = InCubemap.RendererId;
	InCubemap.RendererId = 0;
}

cubemap& cubemap::operator=(cubemap&& InCubemap) noexcept
{
	if (RendererId != 0)
	{
		glDeleteTextures(1, &RendererId);
	}
	RendererId = InCubemap.RendererId;
	InCubemap.RendererId = 0;
	return *this;
}

void cubemap::Load(
	const std::string_view& Directory,
	const std::vector<std::string_view>& TextureFacePaths)
{
	glGenTextures(1, &RendererId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RendererId);

	std::vector<std::string_view> FaceNames = {
		"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"};
	if (!TextureFacePaths.empty())
	{
		FaceNames = TextureFacePaths;
	}

	int32 Width{0};
	int32 Height{0};
	int32 NumChannels{0};
	uint8* LocalBuffer{nullptr};
	for (unsigned int i = 0; i < FaceNames.size(); i++)
	{
		stbi_set_flip_vertically_on_load(0);
		LocalBuffer = stbi_load(
			(std::string(Directory.data()) + "/" + FaceNames[i].data()).c_str(),
			&Width,
			&Height,
			&NumChannels,
			0);
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

void cubemap::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + CubemapSlot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RendererId);
}
