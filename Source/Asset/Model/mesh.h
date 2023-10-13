#pragma once

#include "core_types.h"
#include "Game/Entities/phong_material.h"
#include "Rendering/OldRender/vertex_array.h"
#include "Rendering/OldRender/vertex.h"

struct mesh {
	dyn_array<vertex> mVertices{};
	dyn_array<u32> mIndices{};
	phong_material mMaterial;

	vertex_array mVertexArray{};

	void Init();
};
