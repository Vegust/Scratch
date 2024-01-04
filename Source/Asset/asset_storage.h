#pragma once

#include "Core/String/atom.h"
#include "asset_info.h"
#include "Asset/Model/model.h"
#include "Asset/Texture/texture.h"
#include "Asset/Shader/shader.h"

class asset_storage {
	hash_table<atom, model> mModels;
	hash_table<atom, texture> mTextures;
	hash_table<atom, texture> mShaders;
};