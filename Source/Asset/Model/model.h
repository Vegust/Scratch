#pragma once

#include "core_types.h"
#include "mesh.h"
#include "atom.h"

struct model {
	atom mName;
	dyn_array<mesh> mMeshes;
	str mDirectory;

	void Load(const str& Path);
};
