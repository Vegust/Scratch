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
	mUniformCache = std::move(Shader.mUniformCache);
	Shader.mRendererId = 0;
	Shader.mPath = {};
	Shader.mUniformCache.Clear();
}

shader& shader::operator=(shader&& Shader) noexcept {
	if (mRendererId != 0) {
		glDeleteProgram(mRendererId);
	}
	mRendererId = Shader.mRendererId;
	mPath = std::move(Shader.mPath);
	mUniformCache = std::move(Shader.mUniformCache);
	Shader.mRendererId = 0;
	Shader.mPath = {};
	Shader.mUniformCache.Clear();
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

void shader::SetUniform(str_view Name, float V1, float V2, float V3, float V4) const {
	glUniform4f(GetUniformLocation(Name), V1, V2, V3, V4);
}

void shader::SetUniform(str_view Name, s32 V1) const {
	glUniform1i(GetUniformLocation(Name), V1);
}

void shader::SetUniform(str_view Name, const glm::mat4& Matrix) const {
	glUniformMatrix4fv(GetUniformLocation(Name), 1, GL_FALSE, glm::value_ptr(Matrix));
}

void shader::SetUniform(str_view Name, span<glm::mat4> Matrix) const {
	for (s32 i = 0; i < Matrix.Size(); ++i) {
		glUniformMatrix4fv(GetUniformLocation(Name, i), 1, GL_FALSE, glm::value_ptr(Matrix[i]));
	}
}

void shader::SetUniform(str_view Name, const phong_material& Material) const {
	glUniform1i(GetUniformLocation(Name, "DiffuseMap"), static_cast<s32>(Material.mDiffuseSlot));
	glUniform1i(GetUniformLocation(Name, "SpecularMap"), static_cast<s32>(Material.mSpecularSlot));
	glUniform1i(GetUniformLocation(Name, "EmissionMap"), static_cast<s32>(Material.mEmissionSlot));
	glUniform1i(GetUniformLocation(Name, "NormalMap"), static_cast<s32>(Material.mNormalSlot));
	glUniform1f(GetUniformLocation(Name, "Shininess"), Material.mShininess);
}

void shader::SetUniform(
	str_view Name,
	str_view CountName,
	span<light> Lights,
	const glm::mat4& View) const {
	for (s32 i = 0; i < Lights.Size(); ++i) {
		SetUniform(Name, Lights[i], View, i);
	}
	SetUniform(CountName, static_cast<s32>(Lights.Size()));
}

void shader::SetUniform(str_view Name, const class light& Light, const glm::mat4& View, s32 Index)
	const {
	glUniform1i(GetUniformLocation(Name, Index, "Type"), static_cast<s32>(Light.mType));
	glUniform3fv(GetUniformLocation(Name, Index, "Ambient"), 1, glm::value_ptr(Light.mAmbient));
	glUniform3fv(GetUniformLocation(Name, Index, "Diffuse"), 1, glm::value_ptr(Light.mDiffuse));
	glUniform3fv(GetUniformLocation(Name, Index, "Specular"), 1, glm::value_ptr(Light.mSpecular));
	if (Light.mType == light_type::point) {
		glUniform3fv(
			GetUniformLocation(Name, Index, "Position"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.mPosition, 1.f))));
		glUniform3fv(
			GetUniformLocation(Name, Index, "PositionWorld"), 1, glm::value_ptr(Light.mPosition));
		glUniform1f(GetUniformLocation(Name, Index, "AttenuationRadius"), Light.mAttenuationRadius);
	} else if (Light.mType == light_type::directional) {
		glUniform3fv(
			GetUniformLocation(Name, Index, "Direction"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.mDirection, 0.f))));
	} else if (Light.mType == light_type::spot) {
		glUniform3fv(
			GetUniformLocation(Name, Index, "Position"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.mPosition, 1.f))));
		glUniform3fv(
			GetUniformLocation(Name, Index, "Direction"),
			1,
			glm::value_ptr(glm::vec3(View * glm::vec4(Light.mDirection, 0.f))));
		glUniform1f(GetUniformLocation(Name, Index, "AttenuationRadius"), Light.mAttenuationRadius);
		glUniform1f(
			GetUniformLocation(Name, Index, "AngularAttenuation"), Light.mAngularAttenuation);
		glUniform1f(
			GetUniformLocation(Name, Index, "AngularAttenuationFalloffStart"),
			Light.mAngularAttenuationFalloffStart);
	}
}

void shader::SetUniform(str_view Name, const glm::mat3& Matrix) const {
	glUniformMatrix3fv(GetUniformLocation(Name), 1, GL_FALSE, glm::value_ptr(Matrix));
}

void shader::SetUniform(str_view Name, float V1) const {
	glUniform1f(GetUniformLocation(Name), V1);
}

void shader::SetUniform(str_view Name, glm::vec3 V1) const {
	glUniform3f(GetUniformLocation(Name), V1.x, V1.y, V1.z);
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

s32 shader::GetUniformLocation(str_view First, s32 Index, str_view Second) const {
	const auto Hash = uniform_identifier::hasher::Hash(First, Index, Second);
	if (const auto* Existing =
			mUniformCache.FindByPredicate(Hash, [First, Index, Second](auto& Identifier) {
				return Identifier.mKey.mFirstName == First && Identifier.mKey.mIndex == Index &&
					   Identifier.mKey.mSecondName == Second;
			})) {
		return Existing->mValue;
	}
	str FirstString{First};
	str FullName = FirstString;
	if (Index != -1) {
		FullName += '[' + str{Index} + ']';
	}
	str SecondString{Second};
	if (!Second.Empty()) {
		FullName += '.' + SecondString;
	}
	const auto Location = glGetProgramResourceLocation(mRendererId, GL_UNIFORM, FullName.Raw());
	mUniformCache[uniform_identifier{std::move(FirstString), Index, std::move(SecondString)}] = Location;
	return Location;
}

s32 shader::GetUniformLocation(str_view First, str_view Second) const {
	return GetUniformLocation(First, -1, Second);
}
