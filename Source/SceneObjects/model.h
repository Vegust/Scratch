//
// Created by Vegust on 28.06.2023.
//

#pragma once

#include "mesh.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
SCRATCH_DISABLE_WARNINGS_END()

struct model
{
	std::vector<mesh> Meshes;
	std::filesystem::path Directory;

	void Draw(const renderer& Renderer, const glm::mat4& Transform);
	void Load(std::string_view Path);

	void ProcessNode(aiNode* Node, const aiScene* Scene);
	mesh ProcessMesh(aiMesh* MeshData, const aiScene* Scene);
	std::vector<texture> LoadTextures(
		aiMaterial* Material,
		aiTextureType Type);
};
