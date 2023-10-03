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

public:
	struct uniform_identifier {
		str mFirstName{};
		s32 mIndex{-1};
		str mSecondName{};

		struct hasher {
			[[nodiscard]] FORCEINLINE static hash::hash_type Hash(
				const uniform_identifier& Identifier) {
				return Hash(Identifier.mFirstName, Identifier.mIndex, Identifier.mSecondName);
			}

			[[nodiscard]] FORCEINLINE static hash::hash_type Hash(
				str_view First,
				const s32 Index,
				str_view Second) {
				return hash::HashCombine(
					hash::HashCombine(hash::Hash(First), hash::Hash(Second)), hash::Hash(Index));
			}
		};

		FORCEINLINE bool operator==(const uniform_identifier& Other) const {
			return Other.mIndex == mIndex && Other.mFirstName == mFirstName &&
				   Other.mSecondName == mSecondName;
		}

		[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
			return hasher::Hash(*this);
		}
	};

	mutable hash_table<uniform_identifier, s32> mUniformCache;

	[[nodiscard]] s32 GetUniformLocation(str_view First, s32 Index = -1, str_view Second = {}) const;
	[[nodiscard]] s32 GetUniformLocation(str_view First, str_view Second) const;

	shader() = default;
	~shader();

	shader(const shader&) = delete;
	shader& operator=(const shader&) = delete;

	shader(shader&& Shader) noexcept;
	shader& operator=(shader&& Shader) noexcept;

	void Compile(const str& Path);
	void Bind() const;

	// Set uniforms
	void SetUniform(str_view Name, float V1, float V2, float V3, float V4) const;
	void SetUniform(str_view Name, s32 V1) const;
	void SetUniform(str_view Name, float V1) const;
	void SetUniform(str_view Name, glm::vec3 V1) const;
	void SetUniform(str_view Name, const glm::mat4& Matrix) const;
	void SetUniform(str_view Name, span<glm::mat4> Matrix) const;
	void SetUniform(str_view Name, const glm::mat3& Matrix) const;
	void SetUniform(str_view Name, const phong_material& Material) const;
	void SetUniform(str_view Name, const light& Light, const glm::mat4& View, s32 Index = -1) const;
	void SetUniform(str_view Name, str_view CountName, span<light> Lights, const glm::mat4& View)
		const;

private:
	struct parsed_shaders {
		str mVertexShader{};
		str mGeometryShader{};
		str mFragmentShader{};
	};

	static parsed_shaders ParseShader(const str& Path);
	static u32 CompileShader(u32 Type, const str& Source);
};
