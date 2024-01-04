#pragma once

#include "Core/String/str.h"
#include "Application/Platform/filesystem_types.h"

namespace windows_filesystem {
	// NOTE: in those functions, data from str_view is copied to str and only then passed to file streams
	// because str_view is not guaranteed to be null terminated and std api is garbage
	// TODO: add error info
	dyn_array<u8> LoadRawFile(str_view Filepath);
	str LoadTextFile(str_view Filepath);
	bool SaveRawFile(str_view Filepath, span<u8> Data);
	bool SaveTextFile(str_view Filepath, str_view Data);
};