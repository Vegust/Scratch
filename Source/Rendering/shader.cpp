//
// Created by Vegust on 21.06.2023.
//

#include "shader.h"

#include "SceneObjects/light.h"
#include "SceneObjects/phong_material.h"

#include <string>

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
SCRATCH_DISABLE_WARNINGS_END()

#include <fstream>
#include <iostream>

shader::shader(shader&& InShader)
{
	RendererId = InShader.RendererId;
	Path = std::move(InShader.Path);
	UniformsCache = std::move(InShader.UniformsCache);
	InShader.RendererId = 0;
	InShader.Path = std::filesystem::path{};
	InShader.UniformsCache.clear();
}

shader& shader::operator=(shader&& InShader)
{
	if (RendererId != 0)
	{
		glDeleteProgram(RendererId);
	}
	RendererId = InShader.RendererId;
	Path = std::move(InShader.Path);
	UniformsCache = std::move(InShader.UniformsCache);
	InShader.RendererId = 0;
	InShader.Path = std::filesystem::path{};
	InShader.UniformsCache.clear();
	return *this;
}

void shader::Compile(const std::filesystem::path& InPath)
{
	if (RendererId == 0)
	{
		RendererId = glCreateProgram();
	}

	Path = InPath;
	auto ParsedShaders = ParseShader(InPath);

	auto VSIndex = CompileShader(GL_VERTEX_SHADER, ParsedShaders.VertexShader);
	auto FSIndex = CompileShader(GL_FRAGMENT_SHADER, ParsedShaders.FragmentShader);

	glAttachShader(RendererId, VSIndex);
	glAttachShader(RendererId, FSIndex);
	glLinkProgram(RendererId);
	glValidateProgram(RendererId);

	glDeleteShader(VSIndex);
	glDeleteShader(FSIndex);
}

shader::~shader()
{
	if (RendererId != 0)
	{
		glDeleteProgram(RendererId);
	}
}

void shader::Bind() const
{
	glUseProgram(RendererId);
}

void shader::SetUniform(std::string_view Name, float V1, float V2, float V3, float V4) const
{
	glUniform4f(GetUniformLocation(Name), V1, V2, V3, V4);
}

void shader::SetUniform(std::string_view Name, int32 V1) const
{
	glUniform1i(GetUniformLocation(Name), V1);
}

void shader::SetUniform(std::string_view Name, const glm::mat4& Matrix) const
{
	glUniformMatrix4fv(GetUniformLocation(Name), 1, GL_FALSE, glm::value_ptr(Matrix));
}

void shader::SetUniform(std::string_view Name, const phong_material& Material) const
{
	glUniform1i(
		GetUniformLocation(std::string(Name) + ".DiffuseMap"),
		static_cast<int32>(Material.DiffuseSlot));
	glUniform1i(
		GetUniformLocation(std::string(Name) + ".SpecularMap"),
		static_cast<int32>(Material.SpecularSlot));
	glUniform1i(
		GetUniformLocation(std::string(Name) + ".EmissionMap"),
		static_cast<int32>(Material.EmissionSlot));
	glUniform1f(GetUniformLocation(std::string(Name) + ".Shininess"), Material.Shininess);
}

void shader::SetUniform(
	std::string_view Name,
	std::string_view CountName,
	const std::vector<light>& Lights,
	const glm::mat4& View) const
{
	for (uint32 i = 0; i < Lights.size(); ++i)
	{
		SetUniform(std::string(Name) + "[" + std::to_string(i) + "]", Lights[i], View);
	}
	SetUniform(CountName, static_cast<int32>(Lights.size()));
}

void shader::SetUniform(std::string_view Name, const class light& Light, const glm::mat4& View) const
{
	glUniform1i(GetUniformLocation(std::string(Name) + ".Type"), static_cast<int32>(Light.Type));
	glUniform3fv(
		GetUniformLocation(std::string(Name) + ".Ambient"), 1, glm::value_ptr(Light.Ambient));
	glUniform3fv(
		GetUniformLocation(std::string(Name) + ".Diffuse"), 1, glm::value_ptr(Light.Diffuse));
	glUniform3fv(
		GetUniformLocation(std::string(Name) + ".Specular"), 1, glm::value_ptr(Light.Specular));

	if (Light.Type == light_type::point)
	{
		glUniform3fv(
			GetUniformLocation(std::string(Name) + ".Position"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.Position, 1.f))));
		glUniform1f(
			GetUniformLocation(std::string(Name) + ".AttenuationRadius"), Light.AttenuationRadius);
	}
	else if (Light.Type == light_type::directional)
	{
		glUniform3fv(
			GetUniformLocation(std::string(Name) + ".Direction"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.Direction, 0.f))));
	}
	else if (Light.Type == light_type::spot)
	{
		glUniform3fv(
			GetUniformLocation(std::string(Name) + ".Position"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.Position, 1.f))));
		glUniform3fv(
			GetUniformLocation(std::string(Name) + ".Direction"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.Direction, 0.f))));
		glUniform1f(
			GetUniformLocation(std::string(Name) + ".AttenuationRadius"), Light.AttenuationRadius);
		glUniform1f(
			GetUniformLocation(std::string(Name) + ".AngularAttenuation"),
			Light.AngularAttenuation);
		glUniform1f(
			GetUniformLocation(std::string(Name) + ".AngularAttenuationFalloffStart"),
			Light.AngularAttenuationFalloffStart);
	}
}

void shader::SetUniform(std::string_view Name, const glm::mat3& Matrix) const
{
	glUniformMatrix3fv(GetUniformLocation(Name), 1, GL_FALSE, glm::value_ptr(Matrix));
}

void shader::SetUniform(std::string_view Name, float V1) const
{
	glUniform1f(GetUniformLocation(Name), V1);
}

void shader::SetUniform(std::string_view Name, glm::vec3 V1) const
{
	glUniform3f(GetUniformLocation(Name), V1.x, V1.y, V1.z);
}

int32 shader::GetUniformLocation(std::string_view Name) const
{
	if (const auto Result = UniformsCache.find(Name); Result != UniformsCache.end())
	{
		return Result->second;
	}
	const auto Location = glGetUniformLocation(RendererId, Name.data());
	UniformsCache[std::string{Name}] = Location;
	return Location;
}

shader::parsed_shaders shader::ParseShader(const std::filesystem::path& Path)
{
	std::ifstream InputFile(Path.string(), std::ios::in);
	std::string Line;
	parsed_shaders Result;
	std::optional<size_t> ShaderIndex{};
	while (getline(InputFile, Line))
	{
		if (Line.find("!shader") != std::string::npos)
		{
			if (Line.find("vertex") != std::string::npos)
			{
				ShaderIndex = 0;
			}
			else if (Line.find("fragment") != std::string::npos)
			{
				ShaderIndex = 1;
			}
		}
		else
		{
			if (ShaderIndex.has_value())
			{
				if (ShaderIndex.value() == 0)
				{
					Result.VertexShader.append(Line + "\n");
				}
				if (ShaderIndex.value() == 1)
				{
					Result.FragmentShader.append(Line + "\n");
				}
			}
		}
	}

	return Result;
}

uint32 shader::CompileShader(uint32 Type, std::string_view Source)
{
	auto Index = glCreateShader(Type);
	const char* SourceData = Source.data();
	glShaderSource(Index, 1, &SourceData, nullptr);
	glCompileShader(Index);

	int32 Result;
	glGetShaderiv(Index, GL_COMPILE_STATUS, &Result);
	if (Result == GL_FALSE)
	{
		int32 Length;
		glGetShaderiv(Index, GL_INFO_LOG_LENGTH, &Length);
		char* Message = static_cast<char*>(alloca(static_cast<uint64>(Length) * sizeof(char)));
		glGetShaderInfoLog(Index, Length, &Length, Message);
		std::cout << Message << "\r\n";
		glDeleteShader(Index);
		return 0;
	}

	return Index;
}
