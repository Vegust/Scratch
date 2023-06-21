//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"

#include <map>
#include <type_traits>
#include <filesystem>

class shader
{
private:
	std::filesystem::path Path;
	uint32 RendererId{0};
	mutable std::map<std::string, int32, std::less<>> UniformsCache;
public:
	explicit shader(const std::filesystem::path& InPath);
	shader();
	
	void Bind() const;
	void Unbind() const;
	
	// Set uniforms
	void SetUniform4f(std::string_view Name, float V1, float V2, float V3, float V4);
	void SetUniform1i(std::string_view Name, int32 V1);
private:
	struct parsed_shaders
	{
		std::string VertexShader;
		std::string FragmentShader;
	};
	
	[[nodiscard]] int32 GetUniformLocation(std::string_view Name) const;
	static parsed_shaders ParseShader(const std::filesystem::path& Path);
	static uint32 CompileShader(uint32 Type, std::string_view Source);
	static uint32 CreateShader(std::string_view VertexShader, std::string_view FragmentShader);
};