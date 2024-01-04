#pragma once

#include "basic.h"
#include "mesh.h"
#include "Core/String/atom.h"

struct model {
	atom mName;
	dyn_array<mesh> mMeshes;
	str mDirectory;

	void Load(const str& Path);
};
