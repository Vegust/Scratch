#pragma once

#include "core_types.h"
#include "mesh.h"

struct model {
	dyn_array<mesh> mMeshes;
	str mDirectory;

	void Draw(const old_rebderer& Renderer, const glm::mat4& Transform);
	void Load(const str& Path);
};
