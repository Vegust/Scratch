//
// Created by Vegust on 21.06.2023.
//

#include "texture.h"

#include "core_types.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include "stb_image.h"
SCRATCH_DISABLE_WARNINGS_END()

texture::texture(texture&& InTexture) noexcept
{
	RendererId = InTexture.RendererId;
	InTexture.RendererId = 0;
}

texture& texture::operator=(texture&& InTexture) noexcept
{
	glDeleteTextures(1, &RendererId);
	RendererId = InTexture.RendererId;
	InTexture.RendererId = 0;
	return *this;
}

void texture::Load(std::string_view InPath)
{
	Path = InPath;
	stbi_set_flip_vertically_on_load(1);
	LocalBuffer = stbi_load(InPath.data(), &Width, &Height, &NumChannels, 0);

	if (LocalBuffer)
	{
		if (RendererId == 0)
		{
			glGenTextures(1, &RendererId);
		}
		
		GLenum Format = GL_RGBA;
		if (NumChannels == 1)
		{
			Format = GL_RED;
		}
		else if (NumChannels == 3)
		{
			Format = GL_RGB;
		}
		else if (NumChannels == 4)
		{
			Format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, RendererId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(
			GL_TEXTURE_2D, 0, static_cast<GLint>(Format), Width, Height, 0, Format, GL_UNSIGNED_BYTE, LocalBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(LocalBuffer);
	}
}

texture::~texture()
{
	if (RendererId != 0)
	{
		glDeleteTextures(1, &RendererId);
	}
}

void texture::Bind(uint32 Slot) const
{
	glActiveTexture(GL_TEXTURE0 + Slot);
	glBindTexture(GL_TEXTURE_2D, RendererId);
}
