//
// Created by Vegust on 26.06.2023.
//

#pragma once

#include "Rendering/texture.h"
#include "core_types.h"

#include <memory>

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/glm.hpp"
SCRATCH_DISABLE_WARNINGS_END()

class phong_material
{
public:
	std::unique_ptr<texture> DiffuseMap{nullptr};
	std::unique_ptr<texture> SpecularMap{nullptr};

	uint32 DiffuseSlot = 0;
	uint32 SpecularSlot = 1;

	float Shininess{32.f};

	void InitTextures(
		std::string_view DiffusePath,
		uint32 InDiffuseSlot,
		std::string_view SpecularPath,
		uint32 InSpecularSlot)
	{
		DiffuseMap = std::make_unique<texture>(DiffusePath);
		SpecularMap = std::make_unique<texture>(SpecularPath);
		DiffuseSlot = InDiffuseSlot;
		SpecularSlot = InSpecularSlot;
		
		// Order is important. First, create all, then bind all.
		// because creating textures binds them to current active texture
		DiffuseMap->Bind(DiffuseSlot);
		SpecularMap->Bind(SpecularSlot);
	}
};
