#include "shader.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Containers/span.h"

#include <fstream>
#include <iostream>

shader::shader(shader&& Shader) {
	mRendererId = Shader.mRendererId;
	mPath = std::move(Shader.mPath);
	mUniformsCache = std::move(Shader.mUniformsCache);
	Shader.mRendererId = 0;
	Shader.mPath = {};
	Shader.mUniformsCache.Clear();
}

shader& shader::operator=(shader&& Shader) {
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

void shader::SetUniform(const str& Name, float V1, float V2, float V3, float V4) const {
	glUniform4f(GetUniformLocation(Name), V1, V2, V3, V4);
}

void shader::SetUniform(const str& Name, s32 V1) const {
	glUniform1i(GetUniformLocation(Name), V1);
}

void shader::SetUniform(const str& Name, const glm::mat4& Matrix) const {
	glUniformMatrix4fv(GetUniformLocation(Name), 1, GL_FALSE, glm::value_ptr(Matrix));
}

void shader::SetUniform(const str& Name, const span<glm::mat4>& Matrix) const {
	for (u32 i = 0; i < Matrix.Size(); ++i) {
		glUniformMatrix4fv(
			GetUniformLocation(Name + "[" + str{i} + "]"), 1, GL_FALSE, glm::value_ptr(Matrix[i]));
	}
}

void shader::SetUniform(const str& Name, const phong_material& Material) const {
	glUniform1i(GetUniformLocation(Name + ".DiffuseMap"), static_cast<s32>(Material.mDiffuseSlot));
	glUniform1i(
		GetUniformLocation(Name + ".SpecularMap"), static_cast<s32>(Material.mSpecularSlot));
	glUniform1i(
		GetUniformLocation(Name + ".EmissionMap"), static_cast<s32>(Material.mEmissionSlot));
	glUniform1i(GetUniformLocation(Name + ".NormalMap"), static_cast<s32>(Material.mNormalSlot));
	glUniform1f(GetUniformLocation(Name + ".Shininess"), Material.mShininess);
}

void shader::SetUniform(
	const str& Name,
	const str& CountName,
	const span<light>& Lights,
	const glm::mat4& View) const {
	for (u32 i = 0; i < Lights.Size(); ++i) {
		SetUniform(Name + "[" + str{i} + "]", Lights[i], View);
	}
	SetUniform(CountName, static_cast<s32>(Lights.Size()));
}

void shader::SetUniform(const str& Name, const class light& Light, const glm::mat4& View) const {
	glUniform1i(GetUniformLocation(Name + ".Type"), static_cast<s32>(Light.mType));
	glUniform3fv(GetUniformLocation(Name + ".Ambient"), 1, glm::value_ptr(Light.mAmbient));
	glUniform3fv(GetUniformLocation(Name + ".Diffuse"), 1, glm::value_ptr(Light.mDiffuse));
	glUniform3fv(GetUniformLocation(Name + ".Specular"), 1, glm::value_ptr(Light.mSpecular));

	if (Light.mType == light_type::point) {
		glUniform3fv(
			GetUniformLocation(Name + ".Position"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.mPosition, 1.f))));
		glUniform3fv(
			GetUniformLocation(Name + ".PositionWorld"), 1, glm::value_ptr(Light.mPosition));
		glUniform1f(GetUniformLocation(Name + ".AttenuationRadius"), Light.mAttenuationRadius);
	} else if (Light.mType == light_type::directional) {
		glUniform3fv(
			GetUniformLocation(Name + ".Direction"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.mDirection, 0.f))));
	} else if (Light.mType == light_type::spot) {
		glUniform3fv(
			GetUniformLocation(Name + ".Position"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.mPosition, 1.f))));
		glUniform3fv(
			GetUniformLocation(Name + ".Direction"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.mDirection, 0.f))));
		glUniform1f(GetUniformLocation(Name + ".AttenuationRadius"), Light.mAttenuationRadius);
		glUniform1f(GetUniformLocation(Name + ".AngularAttenuation"), Light.mAngularAttenuation);
		glUniform1f(
			GetUniformLocation(Name + ".AngularAttenuationFalloffStart"),
			Light.mAngularAttenuationFalloffStart);
	}
}

void shader::SetUniform(const str& Name, const glm::mat3& Matrix) const {
	glUniformMatrix3fv(GetUniformLocation(Name), 1, GL_FALSE, glm::value_ptr(Matrix));
}

void shader::SetUniform(const str& Name, float V1) const {
	glUniform1f(GetUniformLocation(Name), V1);
}

void shader::SetUniform(const str& Name, glm::vec3 V1) const {
	glUniform3f(GetUniformLocation(Name), V1.x, V1.y, V1.z);
}

s32 shader::GetUniformLocation(const str& Name) const {
	if (const auto Result = mUniformsCache.Find(Name)) {
		return *Result;
	}
	const auto Location = glGetProgramResourceLocation(mRendererId, GL_UNIFORM, Name.Raw());
	mUniformsCache[Name] = Location;
	return Location;
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
		CHECK(false);
		return 0;
	}

	return Index;
}
