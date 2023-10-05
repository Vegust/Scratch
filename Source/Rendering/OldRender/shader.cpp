#include "shader.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Containers/span.h"

#include <fstream>
#include <iostream>

shader::shader(shader&& Shader) noexcept {
	mRendererId = Shader.mRendererId;
	mPath = std::move(Shader.mPath);
	mUniformsCache = std::move(Shader.mUniformsCache);
	Shader.mRendererId = 0;
	Shader.mPath = {};
	Shader.mUniformsCache.Clear();
}

shader& shader::operator=(shader&& Shader) noexcept {
	if (mRendererId != 0) {
		glDeleteProgram(mRendererId);
	}
	mRendererId = Shader.mRendererId;
	mPath = std::move(Shader.mPath);
	mUniformsCache = std::move(Shader.mUniformsCache);
	Shader.mRendererId = 0;
	Shader.mPath = {};
	Shader.mUniformsCache.Clear();
	return *this;
}

void shader::Compile(const str& Path) {
	if (mRendererId == 0) {
		mRendererId = glCreateProgram();
	}

	mPath = Path;
	auto ParsedShaders = ParseShader(Path);

	auto VSIndex = CompileShader(GL_VERTEX_SHADER, ParsedShaders.mVertexShader);
	auto FSIndex = CompileShader(GL_FRAGMENT_SHADER, ParsedShaders.mFragmentShader);
	glAttachShader(mRendererId, VSIndex);
	glAttachShader(mRendererId, FSIndex);

	if (!ParsedShaders.mGeometryShader.Empty()) {
		auto GSIndex = CompileShader(GL_GEOMETRY_SHADER, ParsedShaders.mGeometryShader);
		glAttachShader(mRendererId, GSIndex);
	}

	glLinkProgram(mRendererId);
	glValidateProgram(mRendererId);

	glDeleteShader(VSIndex);
	glDeleteShader(FSIndex);
}

shader::~shader() {
	if (mRendererId != 0) {
		glDeleteProgram(mRendererId);
	}
}

void shader::Bind() const {
	glUseProgram(mRendererId);
}

void shader::SetUniform(str_view Name, s32 V1) const {
	MaybeSetUniform(V1, Name);
}

void shader::SetUniform(str_view Name, const glm::mat4& Matrix) const {
	MaybeSetUniform(Matrix, Name);
}

void shader::SetUniform(str_view Name, span<glm::mat4> Matrix) const {
	for (s32 i = 0; i < Matrix.Size(); ++i) {
		MaybeSetUniform(Matrix[i], Name, i);
	}
}

void shader::SetUniform(str_view Name, const phong_material& Material) const {
	MaybeSetUniform(static_cast<s32>(Material.mDiffuseSlot), Name, "DiffuseMap");
	MaybeSetUniform(static_cast<s32>(Material.mSpecularSlot), Name, "SpecularMap");
	MaybeSetUniform(static_cast<s32>(Material.mEmissionSlot), Name, "EmissionMap");
	MaybeSetUniform(static_cast<s32>(Material.mNormalSlot), Name, "NormalMap");
	MaybeSetUniform(Material.mShininess, Name, "Shininess");
}

void shader::SetUniform(str_view Name, str_view CountName, span<light> Lights, const glm::mat4& View)
	const {
	for (s32 i = 0; i < Lights.Size(); ++i) {
		SetUniform(Name, Lights[i], View, i);
	}
	SetUniform(CountName, static_cast<s32>(Lights.Size()));
}

void shader::SetUniform(str_view Name, const class light& Light, const glm::mat4& View, s32 Index)
	const {
	glm::vec3 Position = glm::vec3(View * glm::vec4(Light.mPosition, 1.f));
	glm::vec3 Direction = glm::vec3(View * glm::vec4(Light.mDirection, 0.f));
	MaybeSetUniform(static_cast<s32>(Light.mType), Name, Index, "Type");
	MaybeSetUniform(Light.mColor, Name, Index, "Color");
	MaybeSetUniform(Light.mAmbientStrength, Name, Index, "AmbientStrength");
	if (Light.mType == light_type::point) {
		MaybeSetUniform(Position, Name, Index, "Position");
		MaybeSetUniform(Light.mPosition, Name, Index, "PositionWorld");
		MaybeSetUniform(Light.mAttenuationRadius, Name, Index, "AttenuationRadius");
	} else if (Light.mType == light_type::directional) {
		MaybeSetUniform(Direction, Name, Index, "Direction");
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
	String.Clear(false);
	char Char;
	while (Stream.get(Char)) {
		if (Char == '\n') {
			break;
		}
		String += Char;
	}
	return Stream;
}

shader::parsed_shaders shader::ParseShader(const str& Path) {
	std::ifstream InputFile(Path.Raw(), std::ios::in);
	str Line;
	parsed_shaders Result;
	index_type ShaderIndex = InvalidIndex;
	bool bHasGeometryShader = false;
	bool ReachedEOF = InputFile.eof() || !InputFile.is_open();
	while (!ReachedEOF) {
		ReachedEOF = GetLine(InputFile, Line).eof();
		if (Line.Find("!shader") != InvalidIndex && Line.Find("////") == InvalidIndex) {
			if (Line.Find("vertex") != InvalidIndex) {
				ShaderIndex = 0;
			} else if (Line.Find("fragment") != InvalidIndex) {
				ShaderIndex = 1;
			} else if (Line.Find("geometry") != InvalidIndex) {
				bHasGeometryShader = true;
				ShaderIndex = 2;
			} else if (Line.Find("shared") != InvalidIndex) {
				ShaderIndex = 3;
			}
		} else {
			if (ShaderIndex != InvalidIndex) {
				if (ShaderIndex == 0 || ShaderIndex == 3) {
					if (!Result.mVertexShader.Empty() && !Line.Empty()) {
						Result.mVertexShader += '\n';
					}
					Result.mVertexShader += Line;
				}
				if (ShaderIndex == 1 || ShaderIndex == 3) {
					if (!Result.mFragmentShader.Empty() && !Line.Empty()) {
						Result.mFragmentShader += '\n';
					}
					Result.mFragmentShader += Line;
				}
				if (ShaderIndex == 2 || ShaderIndex == 3) {
					if (!Result.mGeometryShader.Empty() && !Line.Empty()) {
						Result.mGeometryShader += '\n';
					}
					Result.mGeometryShader += Line;
				}
			}
		}
	}

	if (!bHasGeometryShader) {
		Result.mGeometryShader = {};
	}

	return Result;
}

u32 shader::CompileShader(u32 Type, const str& Source) {
	auto Index = glCreateShader(Type);
	const char* SourceData = Source.Raw();
	glShaderSource(Index, 1, &SourceData, nullptr);
	glCompileShader(Index);

	s32 Result;
	glGetShaderiv(Index, GL_COMPILE_STATUS, &Result);
	if (Result == GL_FALSE) {
		s32 Length;
		glGetShaderiv(Index, GL_INFO_LOG_LENGTH, &Length);
		char* Message = static_cast<char*>(alloca(static_cast<u64>(Length) * sizeof(char)));
		glGetShaderInfoLog(Index, Length, &Length, Message);
		std::cout << Message << std::endl;
		glDeleteShader(Index);
		CHECK(false)
		return 0;
	}

	return Index;
}

s32 shader::GetUniformLocation(str_view FullName) const {
	return glGetProgramResourceLocation(mRendererId, GL_UNIFORM, FullName.Data());
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