//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"

#include <filesystem>
#include <string_view>

class texture
{
private:
	uint32 RendererId{0};
	std::filesystem::path Path{};
	uint8* LocalBuffer{nullptr};
	int32 Width{0};
	int32 Height{0};
	int32 BitsPerPixel{0};

public:
	explicit texture(std::string_view InPath);
	texture();

	void Bind(uint32 Slot = 0) const;
	void Unbind() const;
};
