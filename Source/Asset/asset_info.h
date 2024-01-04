#pragma once

#include "Containers/hash_table.h"
#include "Core/String/atom.h"

struct asset_info {
	hash_table<atom,str> mInfo;

	asset_info() = default;

	static asset_info Load(str_view Filepath);
	static void Save(str_view Filepath, const asset_info& Info);
};