//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "SceneObjects/light.h"
#include "SceneObjects/phong_material.h"
#include "core_types.h"
#include "glm/fwd.hpp"

#include <filesystem>
#include <map>
#include <type_traits>

class shader
{
private:
	std::filesystem::path Path;
	uint32 RendererId{0};
	mutable std::map<std::string, int32, std::less<>> UniformsCache;
public:
	shader() = default;
	~shader();
	
	shader(const shader&) = delete;
	shader& operator=(const shader&) = delete;
	
	shader(shader&& InShader);
	shader& operator=(shader&& InShader);
	
	void Compile(const std::filesystem::path& InPath);
	void Bind() const;
	
	// Set uniforms
	void SetUniform(std::string_view Name, float V1, float V2, float V3, float V4) const;
	void SetUniform(std::string_view Name, int32 V1) const;
	void SetUniform(std::string_view Name, float V1) const;
	void SetUniform(std::string_view Name, glm::vec3 V1) const;
	void SetUniform(std::string_view Name, const glm::mat4& Matrix) const;
	void SetUniform(std::string_view Name, const glm::mat3& Matrix) const;
	void SetUniform(std::string_view Name, const phong_material& Material) const;
	void SetUniform(std::string_view Name, const light& Light, const glm::mat4& View) const;
	void SetUniform(std::string_view Name, std::string_view CountName, const std::vector<light>& Lights, const glm::mat4& View) const;
private:
	struct parsed_shaders
	{
		std::string VertexShader{};
		std::string GeometryShader{};
		std::string FragmentShader{};
	};
	
	[[nodiscard]] int32 GetUniformLocation(std::string_view Name) const;
	static parsed_shaders ParseShader(const std::filesystem::path& Path);
	static uint32 CompileShader(uint32 Type, std::string_view Source);
};
