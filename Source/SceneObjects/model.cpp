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
		Importer.ReadFile(Path.data(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

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
		aiMesh* MeshData = Scene->mMeshes[Node->mMeshes[i]];
		Meshes.push_back(ProcessMesh(MeshData, Scene));
	}
	for (uint32 i = 0; i < Node->mNumChildren; i++)
	{
		ProcessNode(Node->mChildren[i], Scene);
	}
}

mesh model::ProcessMesh(aiMesh* MeshData, const aiScene* Scene)
{
	mesh OutMesh{};
	OutMesh.Vertices.reserve(MeshData->mNumVertices);
	for (unsigned int i = 0; i < MeshData->mNumVertices; i++)
	{
		vertex Vertex;
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
		OutMesh.Vertices.push_back(Vertex);
	}

	OutMesh.Indices.reserve(MeshData->mNumFaces * 3);
	for (unsigned int i = 0; i < MeshData->mNumFaces; i++)
	{
		aiFace face = MeshData->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			OutMesh.Indices.push_back(face.mIndices[j]);
		}
	}

	aiMaterial* Material = Scene->mMaterials[MeshData->mMaterialIndex];
	
	//TODO: What to do with more than 1 texture per type?
	if (Material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString str;
		Material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		OutMesh.Material.DiffuseMap.Load(Directory.string() + '/' + str.C_Str());
	}
	if (Material->GetTextureCount(aiTextureType_SPECULAR) > 0)
	{
		aiString str;
		Material->GetTexture(aiTextureType_SPECULAR, 0, &str);
		OutMesh.Material.SpecularMap.Load(Directory.string() + '/' + str.C_Str());
	}

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
