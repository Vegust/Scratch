#pragma once

#include "core_types.h"
#include "Rendering/OldRender/texture.h"
#include "Rendering/bind_constants.h"

struct phong_material {
public:
	texture mDiffuseMap{};
	texture mSpecularMap{};
	texture mEmissionMap{};
	texture mNormalMap{};

	float mShininess{32.f};

	void InitTextures(
		const str& DiffusePath = {},
		const str& SpecularPath = {},
		const str& EmissionPath = {},
		const str& NormalPath = {}) {
		if (!DiffusePath.Empty()) {
			mDiffuseMap.Load(DiffusePath, true);
		}
		if (!SpecularPath.Empty()) {
			mSpecularMap.Load(SpecularPath);
		}
		if (!EmissionPath.Empty()) {
			mEmissionMap.Load(EmissionPath);
		}
		if (!NormalPath.Empty()) {
			mNormalMap.Load(NormalPath);
		}
	}

	void Bind() const {
		mDiffuseMap.Bind(DIFFUSE_TEXTURE_SLOT);
		mSpecularMap.Bind(SPECULAR_TEXTURE_SLOT);
		mEmissionMap.Bind(EMISSION_TEXTURE_SLOT);
		mNormalMap.Bind(NORMAL_TEXTURE_SLOT);
	}
};
