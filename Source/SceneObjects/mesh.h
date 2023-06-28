//
// Created by Vegust on 27.06.2023.
//

#pragma once

#include "Rendering/element_buffer.h"
#include "Rendering/texture.h"
#include "Rendering/vertex.h"
#include "Rendering/vertex_array.h"
#include "Rendering/vertex_buffer.h"
#include "core_types.h"
#include "phong_material.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/glm.hpp"
SCRATCH_DISABLE_WARNINGS_END()

#include <vector>

struct mesh
{
	std::vector<vertex> Vertices{};
	std::vector<uint32> Indices{};
	phong_material Material;

	vertex_array VertexArray{};
	vertex_buffer VertexBuffer{};
	element_buffer ElementBuffer{};

	void Init();
	void Draw(const class renderer& Renderer, const glm::mat4& Transform);
};
