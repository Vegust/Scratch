#include "shader.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Containers/span.h"
#include "Logger/logger.h"

#include <fstream>

shader::shader(shader&& Shader) noexcept {
	RendererId = Shader.RendererId;
	Path = std::move(Shader.Path);
	UniformsCache = std::move(Shader.UniformsCache);
	Shader.RendererId = 0;
	Shader.Path.Clear();
	Shader.UniformsCache.Clear();
}

shader& shader::operator=(shader&& Shader) noexcept {
	if (RendererId != 0) {
		glDeleteProgram(RendererId);
	}
	RendererId = Shader.RendererId;
	Path = std::move(Shader.Path);
	UniformsCache = std::move(Shader.UniformsCache);
	Shader.RendererId = 0;
	Shader.Path.Clear();
	Shader.UniformsCache.Clear();
	return *this;
}

void shader::Compile(const str_view InPath) {
	if (RendererId == 0) {
		RendererId = glCreateProgram();
	}

	Path = InPath;
	auto ParsedShaders = ParseShader(InPath);

	auto VSIndex = CompileShader(GL_VERTEX_SHADER, ParsedShaders.VertexShader);
	auto FSIndex = CompileShader(GL_FRAGMENT_SHADER, ParsedShaders.FragmentShader);
	glAttachShader(RendererId, VSIndex);
	glAttachShader(RendererId, FSIndex);

	if (!ParsedShaders.GeometryShader.IsEmpty()) {
		auto GSIndex = CompileShader(GL_GEOMETRY_SHADER, ParsedShaders.GeometryShader);
		glAttachShader(RendererId, GSIndex);
	}

	glLinkProgram(RendererId);
	glValidateProgram(RendererId);

	glDeleteShader(VSIndex);
	glDeleteShader(FSIndex);
}

shader::~shader() {
	if (RendererId != 0) {
		glDeleteProgram(RendererId);
	}
}

void shader::Bind() const {
	glUseProgram(RendererId);
}

void shader::SetUniform(str_view Name, s32 V1) const {
	MaybeSetUniform(V1, Name);
}

void shader::SetUniform(str_view Name, const glm::mat4& Matrix) const {
	MaybeSetUniform(Matrix, Name);
}

void shader::SetUniform(str_view Name, span<glm::mat4> Matrix) const {
	for (s32 i = 0; i < Matrix.GetSize(); ++i) {
		MaybeSetUniform(Matrix[i], Name, i);
	}
}

void shader::SetUniform(str_view Name, const phong_material& Material) const {
	MaybeSetUniform(Material.Shininess, Name, "Shininess");
}

void shader::SetUniform(str_view Name, str_view CountName, span<light> Lights, const glm::mat4& View) const {
	for (s32 i = 0; i < Lights.GetSize(); ++i) {
		SetUniform(Name, Lights[i], View, i);
	}
	SetUniform(CountName, static_cast<s32>(Lights.GetSize()));
}

void shader::SetUniform(str_view Name, const class light& Light, const glm::mat4& View, s32 Index) const {
	glm::vec3 Position = glm::vec3(View * glm::vec4(Light.mPosition, 1.f));
	glm::vec3 Direction = glm::vec3(View * glm::vec4(Light.mDirection, 0.f));
	MaybeSetUniform(static_cast<s32>(Light.mType), Name, Index, "Type");
	MaybeSetUniform(Light.mColor, Name, Index, "Color");
	MaybeSetUniform(Light.mAmbientStrength, Name, Index, "AmbientStrength");
	if (Light.mType == light_type::point) {
		MaybeSetUniform(Position, Name, Index, "Position");
		MaybeSetUniform(Light.mAttenuationRadius, Name, Index, "AttenuationRadius");
	} else if (Light.mType == light_type::directional) {
		MaybeSetUniform(Direction, Name, Index, "Direction");
		MaybeSetUniform(Light.mShadowMatrix, Name, Index, "ShadowMatrix");
	} else if (Light.mType == light_type::spot) {
		MaybeSetUniform(Position, Name, Index, "Position");
		MaybeSetUniform(Direction, Name, Index, "Direction");
		MaybeSetUniform(Light.mAttenuationRadius, Name, Index, "AttenuationRadius");
		MaybeSetUniform(Light.mAngularAttenuation, Name, Index, "AngularAttenuation");
		MaybeSetUniform(Light.mAngularAttenuationFalloffStart, Name, Index, "AngularAttenuationFalloffStart");
	}
}

void shader::SetUniform(str_view Name, const glm::mat3& Matrix) const {
	MaybeSetUniform(Matrix, Name);
}

void shader::SetUniform(str_view Name, float V1) const {
	MaybeSetUniform(V1, Name);
}

void shader::SetUniform(str_view Name, glm::vec3 V1) const {
	MaybeSetUniform(V1, Name);
}

static std::ifstream& GetLine(std::ifstream& Stream, str& String) {
	String.Clear(container_clear_type::dont_deallocate);
	char Char;
	while (Stream.get(Char)) {
		if (Char == '\n') {
			break;
		}
		String += Char;
	}
	return Stream;
}

shader::parsed_shaders shader::ParseShader(const str_view Path) {
	// NOTE: need to convert to string to allocate space for null terminator because garbage std functions
	// TODO: would be nice to have some way to easily allocate arbitrary length string on the stack
	std::ifstream InputFile(str{Path}.GetRaw(), std::ios::in);
	str Line;
	parsed_shaders Result;
	index ShaderIndex = InvalidIndex;
	bool bHasGeometryShader = false;
	bool ReachedEOF = InputFile.eof() || !InputFile.is_open();
	while (!ReachedEOF) {
		ReachedEOF = GetLine(InputFile, Line).eof();
		if (strings::FindSubstring(Line, "!shader") != InvalidIndex &&
			strings::FindSubstring(Line, "////") == InvalidIndex) {
			if (strings::FindSubstring(Line, "vertex") != InvalidIndex) {
				ShaderIndex = 0;
			} else if (strings::FindSubstring(Line, "fragment") != InvalidIndex) {
				ShaderIndex = 1;
			} else if (strings::FindSubstring(Line, "geometry") != InvalidIndex) {
				bHasGeometryShader = true;
				ShaderIndex = 2;
			} else if (strings::FindSubstring(Line, "shared") != InvalidIndex) {
				ShaderIndex = 3;
			}
		} else {
			if (ShaderIndex != InvalidIndex) {
				if (ShaderIndex == 0 || ShaderIndex == 3) {
					if (!Result.VertexShader.IsEmpty() && !Line.IsEmpty()) {
						Result.VertexShader += '\n';
					}
					Result.VertexShader += Line;
				}
				if (ShaderIndex == 1 || ShaderIndex == 3) {
					if (!Result.FragmentShader.IsEmpty() && !Line.IsEmpty()) {
						Result.FragmentShader += '\n';
					}
					Result.FragmentShader += Line;
				}
				if (ShaderIndex == 2 || ShaderIndex == 3) {
					if (!Result.GeometryShader.IsEmpty() && !Line.IsEmpty()) {
						Result.GeometryShader += '\n';
					}
					Result.GeometryShader += Line;
				}
			}
		}
	}

	if (!bHasGeometryShader) {
		Result.GeometryShader.Clear();
	}

	return Result;
}

u32 shader::CompileShader(u32 Type, const str& Source) {
	auto Index = glCreateShader(Type);
	const char* SourceData = Source.GetRaw();
	glShaderSource(Index, 1, &SourceData, nullptr);
	glCompileShader(Index);

	s32 Result;
	glGetShaderiv(Index, GL_COMPILE_STATUS, &Result);
	if (Result == GL_FALSE) {
		s32 Length;
		glGetShaderiv(Index, GL_INFO_LOG_LENGTH, &Length);
		char* Message = static_cast<char*>(alloca(static_cast<u64>(Length) * sizeof(char)));
		glGetShaderInfoLog(Index, Length, &Length, Message);
		logger::Log("%s", Message);
		glDeleteShader(Index);
		CHECK(false)
		return 0;
	}

	return Index;
}

s32 shader::GetUniformLocation(str_view FullName) const {
	return glGetProgramResourceLocation(RendererId, GL_UNIFORM, FullName.GetData());
}

void shader::uniforms_cache::SetValue(s32 ResourceId, s32 Value) {
	glUniform1i(ResourceId, Value);
}

void shader::uniforms_cache::SetValue(s32 ResourceId, float Value) {
	glUniform1f(ResourceId, Value);
}

void shader::uniforms_cache::SetValue(s32 ResourceId, const glm::vec3& Value) {
	glUniform3f(ResourceId, Value.x, Value.y, Value.z);
}

void shader::uniforms_cache::SetValue(s32 ResourceId, const glm::vec4& Value) {
	glUniform4f(ResourceId, Value.x, Value.y, Value.z, Value.w);
}

void shader::uniforms_cache::SetValue(s32 ResourceId, const glm::mat3& Value) {
	glUniformMatrix3fv(ResourceId, 1, GL_FALSE, glm::value_ptr(Value));
}

void shader::uniforms_cache::SetValue(s32 ResourceId, const glm::mat4& Value) {
	glUniformMatrix4fv(ResourceId, 1, GL_FALSE, glm::value_ptr(Value));
}