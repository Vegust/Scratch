//
// Created by Vegust on 21.06.2023.
//

#include "shader.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
SCRATCH_DISABLE_WARNINGS_END()

#include <fstream>
#include <iostream>

shader::shader(const std::filesystem::path& InPath) : Path(InPath)
{
	auto ParsedShaders = ParseShader(InPath);
	RendererId = CreateShader(ParsedShaders.VertexShader, ParsedShaders.FragmentShader);
}

shader::shader()
{
	glDeleteProgram(RendererId);
}

void shader::Bind() const
{
	glUseProgram(RendererId);
}

void shader::Unbind() const
{
	glUseProgram(0);
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

uint32 shader::CreateShader(std::string_view VertexShader, std::string_view FragmentShader)
{
	const auto ProgramIndex = glCreateProgram();
	auto VSIndex = CompileShader(GL_VERTEX_SHADER, VertexShader);
	auto FSIndex = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);

	glAttachShader(ProgramIndex, VSIndex);
	glAttachShader(ProgramIndex, FSIndex);
	glLinkProgram(ProgramIndex);
	glValidateProgram(ProgramIndex);

	glDeleteShader(VSIndex);
	glDeleteShader(FSIndex);

	return ProgramIndex;
}
