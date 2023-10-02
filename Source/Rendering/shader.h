#pragma once

#include "core_types.h"
#include "SceneObjects/light.h"
#include "SceneObjects/phong_material.h"
#include "Containers/span.h"
#include "glm/fwd.hpp"

class shader {
private:
	str mPath;
	u32 mRendererId{0};
	mutable hash_table<str, s32> mUniformsCache;

public:
	shader() = default;
	~shader();

	shader(const shader&) = delete;
	shader& operator=(const shader&) = delete;

	shader(shader&& Shader);
	shader& operator=(shader&& Shader);

	void Compile(const str& Path);
	void Bind() const;

	// Set uniforms
	void SetUniform(const str& Name, float V1, float V2, float V3, float V4) const;
	void SetUniform(const str& Name, s32 V1) const;
	void SetUniform(const str& Name, float V1) const;
	void SetUniform(const str& Name, glm::vec3 V1) const;
	void SetUniform(const str& Name, const glm::mat4& Matrix) const;
	void SetUniform(const str& Name, const span<glm::mat4>& Matrix) const;
	void SetUniform(const str& Name, const glm::mat3& Matrix) const;
	void SetUniform(const str& Name, const phong_material& Material) const;
	void SetUniform(const str& Name, const light& Light, const glm::mat4& View) const;
	void SetUniform(
		const str& Name,
		const str& CountName,
		const dyn_array<light>& Lights,
		const glm::mat4& View) const;

private:
	struct parsed_shaders {
		str mVertexShader{};
		str mGeometryShader{};
		str mFragmentShader{};
	};

	[[nodiscard]] s32 GetUniformLocation(const str& Name) const;
	static parsed_shaders ParseShader(const str& Path);
	static u32 CompileShader(u32 Type, const str& Source);
};
