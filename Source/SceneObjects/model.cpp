//
// Created by Vegust on 28.06.2023.
//

#include "model.h"

#include <iostream>

void model::Draw(const renderer& Renderer, const glm::mat4& Transform)
{
	for (auto& Mesh : Meshes)
	{
		Mesh.Draw(Renderer, Transform);
	}
}

void model::Load(std::string_view Path)
{
	Meshes.clear();

	Assimp::Importer Importer;
	const aiScene* Scene =
		Importer.ReadFile(Path.data(), aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << Importer.GetErrorString() << std::endl;
		return;
	}
	Directory = Path.substr(0, Path.find_last_of('/'));

	ProcessNode(Scene->mRootNode, Scene);
}

void model::ProcessNode(aiNode* Node, const aiScene* Scene)
{
	for (uint32 i = 0; i < Node->mNumMeshes; i++)
	{
		SCRATCH_DISABLE_WARNINGS_BEGIN()
		aiMesh* MeshData = Scene->mMeshes[Node->mMeshes[i]];
		SCRATCH_DISABLE_WARNINGS_END()
		Meshes.push_back(ProcessMesh(MeshData, Scene));
	}
	for (uint32 i = 0; i < Node->mNumChildren; i++)
	{
		SCRATCH_DISABLE_WARNINGS_BEGIN()
		ProcessNode(Node->mChildren[i], Scene);
		SCRATCH_DISABLE_WARNINGS_END()
	}
}

mesh model::ProcessMesh(aiMesh* MeshData, const aiScene* Scene)
{
	mesh OutMesh{};
	OutMesh.Vertices.reserve(MeshData->mNumVertices);
	for (unsigned int i = 0; i < MeshData->mNumVertices; i++)
	{
		vertex Vertex;
		SCRATCH_DISABLE_WARNINGS_BEGIN()
		Vertex.Position.x = MeshData->mVertices[i].x;
		Vertex.Position.y = MeshData->mVertices[i].y;
		Vertex.Position.z = MeshData->mVertices[i].z;
		Vertex.Normal.x = MeshData->mNormals[i].x;
		Vertex.Normal.y = MeshData->mNormals[i].y;
		Vertex.Normal.z = MeshData->mNormals[i].z;
		if (MeshData->mTextureCoords[0])
		{
			Vertex.UV.x = MeshData->mTextureCoords[0][i].x;
			Vertex.UV.y = MeshData->mTextureCoords[0][i].y;
		}
		SCRATCH_DISABLE_WARNINGS_END()
		OutMesh.Vertices.push_back(Vertex);
	}

	OutMesh.Indices.reserve(MeshData->mNumFaces * 3);
	for (unsigned int i = 0; i < MeshData->mNumFaces; i++)
	{
		SCRATCH_DISABLE_WARNINGS_BEGIN()
		aiFace face = MeshData->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			OutMesh.Indices.push_back(face.mIndices[j]);
		}
		SCRATCH_DISABLE_WARNINGS_END()
	}

	SCRATCH_DISABLE_WARNINGS_BEGIN()
	aiMaterial* Material = Scene->mMaterials[MeshData->mMaterialIndex];
	SCRATCH_DISABLE_WARNINGS_END()
	OutMesh.Material.DiffuseMaps = LoadTextures(Material, aiTextureType_DIFFUSE);
	OutMesh.Material.SpecularMaps = LoadTextures(Material, aiTextureType_SPECULAR);

	OutMesh.Init();
	return OutMesh;
}

std::vector<texture> model::LoadTextures(aiMaterial* Material, aiTextureType Type)
{
	std::vector<texture> Textures;
	Textures.reserve(Material->GetTextureCount(Type));
	for (unsigned int i = 0; i < Material->GetTextureCount(Type); i++)
	{
		aiString str;
		Material->GetTexture(Type, i, &str);
		Textures.emplace_back();
		Textures[i].Load(Directory.string() + '/' + str.C_Str());
	}
	return Textures;
}
