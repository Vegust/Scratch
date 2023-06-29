//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"

#include <filesystem>
#include <string_view>
#include <unordered_map>

class texture
{
private:
	uint32 RendererId{0};
	std::filesystem::path Path{};
	uint8* LocalBuffer{nullptr};
	
public:
	texture() = default;
	~texture();
	
	texture(const texture& InTexture) = delete;
	texture& operator=(const texture&) = delete;
	texture(texture&& InTexture) noexcept;
	texture& operator=(texture&& InTexture) noexcept;
	
	static std::unordered_map<std::string, std::pair<uint32, uint32>>& GetTextureCache()
	{
		static std::unordered_map<std::string, std::pair<uint32, uint32>> TextureCache{};
		return TextureCache;
	}
	
	void ClearTextureHandle();

	bool Loaded() const { return RendererId != 0; }
	
	void Load(std::string_view InPath);
	
	void Bind(uint32 Slot = 0) const;
};
