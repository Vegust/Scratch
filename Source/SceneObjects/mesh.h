//
// Created by Vegust on 27.06.2023.
//

#pragma once

#include "Rendering/index_buffer.h"
#include "Rendering/texture.h"
#include "Rendering/vertex.h"
#include "Rendering/vertex_array.h"
#include "Rendering/vertex_buffer.h"
#include "core_types.h"

#include <vector>

struct mesh
{
	std::vector<vertex> Vertices{};
	std::vector<uint32> Indices{};
	std::vector<texture> Textures{};

	vertex_array VAO{};
	vertex_buffer VBO{};
	index_buffer EBO{};

	void Init(
		std::vector<vertex>&& InVertices,
		std::vector<uint32>&& InIndices,
		std::vector<texture>&& InTextures);
	void Draw();
};
