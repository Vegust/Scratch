#pragma once

#include "basic.h"
#include "Asset/Texture/texture.h"
#include "Rendering/bind_constants.h"

struct phong_material {
public:
//	ref<texture> mDiffuseMap{};
//	ref<texture> mSpecularMap{};
//	ref<texture> mEmissionMap{};
//	ref<texture> mNormalMap{};

	texture DiffuseMap{};
	texture SpecularMap{};
	texture EmissionMap{};
	texture NormalMap{};

	float Shininess{32.f};

	void InitTextures(
		//asset_storage& AssetStorage,
		const str& DiffusePath = {},
		const str& SpecularPath = {},
		const str& EmissionPath = {},
		const str& NormalPath = {}) {
		if (!DiffusePath.IsEmpty()) {
			//mDiffuseMap = AssetStorage.Load<texture>(DiffusePath);
			DiffuseMap.Load(DiffusePath, true);
		}
		if (!SpecularPath.IsEmpty()) {
			SpecularMap.Load(SpecularPath);
		}
		if (!EmissionPath.IsEmpty()) {
			EmissionMap.Load(EmissionPath);
		}
		if (!NormalPath.IsEmpty()) {
			NormalMap.Load(NormalPath);
		}
	}

	void Bind() const {
		DiffuseMap.Bind(DIFFUSE_TEXTURE_SLOT);
		SpecularMap.Bind(SPECULAR_TEXTURE_SLOT);
		EmissionMap.Bind(EMISSION_TEXTURE_SLOT);
		NormalMap.Bind(NORMAL_TEXTURE_SLOT);
	}
};
