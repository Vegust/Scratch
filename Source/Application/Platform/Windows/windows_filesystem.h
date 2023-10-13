#pragma once

#include "Containers/str.h"
#include "Application/Platform/filesystem_types.h"

struct windows_filesystem {
	// NOTE: in those functions, data from str_view is copied to str and only then passed to file streams
	// because str_view is not guaranteed to be null terminated and std api is garbage
	// TODO: add error info
	static dyn_array<u8> LoadRawFile(str_view Filepath);
	static str LoadTextFile(str_view Filepath);
	static bool SaveRawFile(str_view Filepath, span<u8> Data);
	static bool SaveTextFile(str_view Filepath, str_view Data);
};