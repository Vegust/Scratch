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
	texture DiffuseMap{};
	texture SpecularMap{};
	texture EmissionMap{};

	uint32 DiffuseSlot = 0;
	uint32 SpecularSlot = 1;
	uint32 EmissionSlot = 2;
	
	float Shininess{32.f};

	void InitTextures(
		std::string_view DiffusePath,
		uint32 InDiffuseSlot,
		std::string_view SpecularPath,
		uint32 InSpecularSlot,
		std::string_view EmissionPath = {},
		uint32 InEmissionSlot = 2)
	{
		DiffuseMap.Load(DiffusePath, true);
		SpecularMap.Load(SpecularPath);
		if (!EmissionPath.empty())
		{
			EmissionMap.Load(EmissionPath);
		}
		
		DiffuseSlot = InDiffuseSlot;
		SpecularSlot = InSpecularSlot;
		EmissionSlot = InEmissionSlot;

		// Order is important. First, create all, then bind all.
		// because creating textures binds them to current active texture
		DiffuseMap.Bind(DiffuseSlot);
		SpecularMap.Bind(SpecularSlot);
		if (EmissionMap.Loaded())
		{
			EmissionMap.Bind(EmissionSlot);
		}
	}
	
	void Bind() const
	{
		DiffuseMap.Bind(DiffuseSlot);
		SpecularMap.Bind(SpecularSlot);
		if (EmissionMap.Loaded())
		{
			EmissionMap.Bind(EmissionSlot);
		}
	}
};
