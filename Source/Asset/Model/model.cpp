#include "model.h"

#include <iostream>

// TODO: remove assimp
#include "ThirdParty/assimp/include/assimp/postprocess.h"
#include "ThirdParty/assimp/include/assimp/scene.h"
#include "ThirdParty/assimp/include/assimp/Importer.hpp"

static void ProcessNode(model& Model, aiNode* Node, const aiScene* Scene);
static mesh ProcessMesh(model& Model, aiMesh* MeshData, const aiScene* Scene);
static dyn_array<texture> LoadTextures(model& Model, aiMaterial* Material, aiTextureType Type);

void model::Load(const str& Path) {
	mMeshes.Clear();

	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(
		Path.GetRaw(),
		aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
			aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << Importer.GetErrorString() << std::endl;
		return;
	}

	index LastBackspace = Path.FindLastOf('/');
	mDirectory = LastBackspace != InvalidIndex ? Path.Substr(0, LastBackspace) : Path;

	ProcessNode(*this, Scene->mRootNode, Scene);
}

void ProcessNode(model& Model, aiNode* Node, const aiScene* Scene) {
	for (u32 i = 0; i < Node->mNumMeshes; i++) {
		aiMesh* MeshData = Scene->mMeshes[Node->mMeshes[i]];
		Model.mMeshes.Emplace(ProcessMesh(Model, MeshData, Scene));
	}
	for (u32 i = 0; i < Node->mNumChildren; i++) {
		ProcessNode(Model, Node->mChildren[i], Scene);
	}
}

mesh ProcessMesh(model& Model, aiMesh* MeshData, const aiScene* Scene) {
	mesh OutMesh{};
	OutMesh.mVertices.Reserve(MeshData->mNumVertices);
	for (unsigned int i = 0; i < MeshData->mNumVertices; i++) {
		vertex Vertex;
		Vertex.Position.x = MeshData->mVertices[i].x;
		Vertex.Position.y = MeshData->mVertices[i].y;
		Vertex.Position.z = MeshData->mVertices[i].z;
		Vertex.Normal.x = MeshData->mNormals[i].x;
		Vertex.Normal.y = MeshData->mNormals[i].y;
		Vertex.Normal.z = MeshData->mNormals[i].z;
		if (MeshData->mTextureCoords[0]) {
			Vertex.UV.x = MeshData->mTextureCoords[0][i].x;
			Vertex.UV.y = MeshData->mTextureCoords[0][i].y;
		}
		OutMesh.mVertices.Add(Vertex);
	}

	OutMesh.mIndices.Reserve(MeshData->mNumFaces * 3);
	for (unsigned int i = 0; i < MeshData->mNumFaces; i++) {
		aiFace face = MeshData->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			OutMesh.mIndices.Add(face.mIndices[j]);
		}
	}

	aiMaterial* Material = Scene->mMaterials[MeshData->mMaterialIndex];

	// TODO: What to do with more than 1 texture per type?
	if (Material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString str;
		Material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		OutMesh.mMaterial.DiffuseMap.Load(Model.mDirectory + '/' + str.C_Str(), true);
	} else {
		OutMesh.mMaterial.DiffuseMap.Load("Assets/Textures/DefaultDiffuse.jpg", true);
	}
	if (Material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
		aiString str;
		Material->GetTexture(aiTextureType_SPECULAR, 0, &str);
		OutMesh.mMaterial.SpecularMap.Load(Model.mDirectory + '/' + str.C_Str());
	} else {
		OutMesh.mMaterial.SpecularMap.Load("Assets/Textures/DefaultSpecular.jpg");
	}

	OutMesh.Init();
	return OutMesh;
}

dyn_array<texture> LoadTextures(model& Model, aiMaterial* Material, aiTextureType Type) {
	dyn_array<texture> Textures;
	Textures.EnsureCapacity(Material->GetTextureCount(Type));
	for (unsigned int i = 0; i < Material->GetTextureCount(Type); i++) {
		aiString str;
		Material->GetTexture(Type, i, &str);
		Textures.Emplace();
		Textures[i].Load(Model.mDirectory + '/' + str.C_Str());
	}
	return Textures;
}
