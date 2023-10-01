#pragma once

#include "core_types.h"
#include "Rendering/texture.h"

struct phong_material {
public:
	texture mDiffuseMap{};
	texture mSpecularMap{};
	texture mEmissionMap{};
	texture mNormalMap{};

	u32 mDiffuseSlot = 0;
	u32 mSpecularSlot = 1;
	u32 mEmissionSlot = 2;
	u32 mNormalSlot = 3;

	float mShininess{32.f};

	void InitTextures(
		const str& DiffusePath,
		u32 DiffuseSlot,
		const str& SpecularPath,
		u32 SpecularSlot,
		const str& EmissionPath = {},
		u32 EmissionSlot = 2,
		const str& NormalPath = {},
		u32 NormalSlot = 3) {
		mDiffuseMap.Load(DiffusePath, true);
		mSpecularMap.Load(SpecularPath);
		if (!EmissionPath.Empty()) {
			mEmissionMap.Load(EmissionPath);
		}
		if (!NormalPath.Empty()) {
			mNormalMap.Load(NormalPath);
		}

		mDiffuseSlot = DiffuseSlot;
		mSpecularSlot = SpecularSlot;
		mEmissionSlot = EmissionSlot;
		mNormalSlot = NormalSlot;

		// Order is important. First, create all, then bind all.
		// because creating textures binds them to current active texture
		mDiffuseMap.Bind(mDiffuseSlot);
		mSpecularMap.Bind(mSpecularSlot);
		if (mEmissionMap.Loaded()) {
			mEmissionMap.Bind(mEmissionSlot);
		}
		if (mNormalMap.Loaded()) {
			mNormalMap.Bind(mNormalSlot);
		}
	}

	void Bind() const {
		mDiffuseMap.Bind(mDiffuseSlot);
		mSpecularMap.Bind(mSpecularSlot);
		if (mEmissionMap.Loaded()) {
			mEmissionMap.Bind(mEmissionSlot);
		}
		if (mNormalMap.Loaded()) {
			mNormalMap.Bind(mNormalSlot);
		}
	}
};
