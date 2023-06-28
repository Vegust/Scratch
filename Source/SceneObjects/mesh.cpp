//
// Created by Vegust on 27.06.2023.
//

#include "mesh.h"

void mesh::Init(
	std::vector<vertex>&& InVertices,
	std::vector<uint32>&& InIndices,
	std::vector<texture>&& InTextures)
{
	Vertices = InVertices;
	Indices = InIndices;
	Textures = std::move(InTextures);
	
	
}

void mesh::Draw()
{
}
