#include "windows_filesystem.h"
#include <fstream>

dyn_array<u8> windows_filesystem::LoadRawFile(str_view Filepath) {
	dyn_array<u8> Result{};
	std::ifstream File{str{Filepath}.GetData(), std::ios::binary};
	if (File.is_open()) {
		File.seekg(0, std::ios::end);
		const std::streamsize Size = File.tellg();
		CHECK(Size < InvalidIndex && Size > 0)
		File.seekg(0, std::ios::beg);
		Result.Reserve(Size);
		File.read((char*) Result.GetData(), Size);
		Result.OverwriteSize(Size);
	}
	return Result;
}

str windows_filesystem::LoadTextFile(str_view Filepath) {
	str Result{};
	std::ifstream File{str{Filepath}.GetData()};
	if (File.is_open()) {
		File.seekg(0, std::ios::end);
		const std::streamsize Size = File.tellg();
		CHECK(Size < InvalidIndex && Size > 0)
		File.seekg(0, std::ios::beg);
		Result.Reserve(Size + 1);
		File.read(Result.GetRaw(), Size);
		Result.OverwriteSize(Size);
		if (Result.GetAt(Size - 1) != 0) {
			Result.GetAt(Size) = 0;
			Result.OverwriteSize(Size + 1);
		}
	}
	return Result;
}

bool windows_filesystem::SaveRawFile(str_view Filepath, span<u8> Data) {
	if (Data.IsEmpty()) {
		// NOTE: maybe create empty file or just return true?
		return false;
	}
	std::ofstream File(str{Filepath}.GetData(), std::ios::binary);
	if (!File.is_open()) {
		return false;
	}
	File.write((const char*) Data.GetData(), Data.GetSize());
	if (File.fail()) {
		return false;
	}
	return true;
}

bool windows_filesystem::SaveTextFile(str_view Filepath, str_view Data) {
	if (Data.IsEmpty()) {
		// NOTE: maybe create empty file or just return true?
		return false;
	}
	std::ofstream File(str{Filepath}.GetData());
	if (!File.is_open()) {
		return false;
	}
	// TODO: make stream subclasses that work with my types or
	// just ditch std and use system calls
	std::string_view GarbageApi{Data.GetData(), Data.GetData() + Data.GetSize() - 1};
	File << GarbageApi;
	if (File.fail()) {
		return false;
	}
	return true;
}
