#pragma once

#include "core_types.h"
#include "mesh.h"

struct model {
	dyn_array<mesh> mMeshes;
	str mDirectory;

	void Load(const str& Path);
};
