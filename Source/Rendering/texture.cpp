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
	Path = std::move(InTexture.Path);
	InTexture.RendererId = 0;
	InTexture.Path = std::filesystem::path{};
}

texture& texture::operator=(texture&& InTexture) noexcept
{
	ClearTextureHandle();
	RendererId = InTexture.RendererId;
	Path = std::move(InTexture.Path);
	InTexture.RendererId = 0;
	InTexture.Path = std::filesystem::path{};
	return *this;
}

void texture::ClearTextureHandle()
{
	if (RendererId != 0)
	{
		auto& Cache = GetTextureCache();
		if (auto Found = Cache.find(Path.string()); Found != Cache.end())
		{
			Found->second.second -= 1;
			if (Found->second.second == 0)
			{
				Cache.erase(Found);
				glDeleteTextures(1, &RendererId);
			}
			RendererId = 0;
			return;
		}
		std::unreachable();
	}
}

void texture::Load(std::string_view InPath, bool bSRGB)
{
	ClearTextureHandle();

	Path = InPath;
	auto& Cache = GetTextureCache();
	if (auto Found = Cache.find(Path.string()); Found != Cache.end())
	{
		Found->second.second += 1;
		RendererId = Found->second.first;
	}
	else
	{
		int32 Width{0};
		int32 Height{0};
		int32 NumChannels{0};
		stbi_set_flip_vertically_on_load(1);
		LocalBuffer = stbi_load(InPath.data(), &Width, &Height, &NumChannels, 4);
		
		if (LocalBuffer)
		{
			glGenTextures(1,&RendererId);
			Cache[Path.string()] = std::make_pair(RendererId,1);
			
			glBindTexture(GL_TEXTURE_2D, RendererId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			
			uint32 InternalType = GL_RGBA8;
			if (bSRGB)
			{
				InternalType = GL_SRGB8_ALPHA8;
			}

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				static_cast<GLint>(InternalType),
				Width,
				Height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				LocalBuffer);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			stbi_image_free(LocalBuffer);
		}
	}
}

texture::~texture()
{
	ClearTextureHandle();
}

void texture::Bind(uint32 Slot) const
{
	glActiveTexture(GL_TEXTURE0 + Slot);
	glBindTexture(GL_TEXTURE_2D, RendererId);
}
