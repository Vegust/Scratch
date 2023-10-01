#pragma once

#include "core_types.h"
#include "Rendering/element_buffer.h"
#include "Rendering/texture.h"
#include "Rendering/vertex.h"
#include "Rendering/vertex_array.h"
#include "Rendering/vertex_buffer.h"
#include "phong_material.h"
#include "glm/glm.hpp"

struct mesh {
	dyn_array<vertex> mVertices{};
	dyn_array<u32> mIndices{};
	phong_material mMaterial;

	vertex_array mVertexArray{};
	vertex_buffer mVertexBuffer{};
	element_buffer mElementBuffer{};

	void Init();
	void Draw(const class renderer& Renderer, const glm::mat4& Transform);
};
