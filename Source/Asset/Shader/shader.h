#pragma once

#include "core.h"
#include "Game/Entities/light.h"
#include "Game/Entities/phong_material.h"
#include "Containers/span.h"
#include "glm/fwd.hpp"
#include "String/str_util.h"

class shader {
public:
	str Path;
	u32 RendererId{0};

	struct uniform_identifier {
		// First + Index + Second = full name string.
		// (e.g. Lights + 2 + Attenuation = Lights[2].Attenuation).
		// Stored separately, so we don't need to do string allocations when
		// setting uniform values
		str FirstName{};
		s32 Index{-1};
		str SecondName{};

		struct hasher {
			[[nodiscard]] FORCEINLINE static hash::hash_type Hash(const uniform_identifier& Identifier) {
				return Hash(Identifier.FirstName, Identifier.Index, Identifier.SecondName);
			}
			
			[[nodiscard]] FORCEINLINE static hash::hash_type Hash(str_view First, const s32 Index, str_view Second) {
				return hash::HashCombine(hash::HashCombine(hash::Hash(First), hash::Hash(Second)), hash::Hash(Index));
			}
		};

		FORCEINLINE bool operator==(const uniform_identifier& Other) const {
			return Other.Index == Index && Other.FirstName == FirstName && Other.SecondName == SecondName;
		}

		[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
			return hasher::Hash(*this);
		}
	};

	mutable struct uniforms_cache {
		template <typename value_type>
		struct cached_uniform {
			s32 ResourceId{0};
			value_type Value{};
		};

		hash_table<uniform_identifier, cached_uniform<s32>> Ints;
		hash_table<uniform_identifier, cached_uniform<float>> Floats;
		hash_table<uniform_identifier, cached_uniform<glm::vec3>> Vec3s;
		hash_table<uniform_identifier, cached_uniform<glm::vec4>> Vec4s;
		hash_table<uniform_identifier, cached_uniform<glm::mat3>> Mat3s;
		hash_table<uniform_identifier, cached_uniform<glm::mat4>> Mat4s;

		template <typename value_type>
		FORCEINLINE auto& GetCache() {
			if constexpr (std::is_same_v<value_type, s32>) {
				return Ints;
			} else if constexpr (std::is_same_v<value_type, float>) {
				return Floats;
			} else if constexpr (std::is_same_v<value_type, glm::vec3>) {
				return Vec3s;
			} else if constexpr (std::is_same_v<value_type, glm::vec4>) {
				return Vec4s;
			} else if constexpr (std::is_same_v<value_type, glm::mat3>) {
				return Mat3s;
			} else if constexpr (std::is_same_v<value_type, glm::mat4>) {
				return Mat4s;
			};
		}

		static void SetValue(s32 ResourceId, s32 Value);
		static void SetValue(s32 ResourceId, float Value);
		static void SetValue(s32 ResourceId, const glm::vec3& Value);
		static void SetValue(s32 ResourceId, const glm::vec4& Value);
		static void SetValue(s32 ResourceId, const glm::mat3& Value);
		static void SetValue(s32 ResourceId, const glm::mat4& Value);

		FORCEINLINE void Clear() {
			Ints.Clear();
			Floats.Clear();
			Vec3s.Clear();
			Vec4s.Clear();
			Mat3s.Clear();
			Mat4s.Clear();
		}

	} UniformsCache;

	shader() = default;
	~shader();

	shader(const shader&) = delete;
	shader& operator=(const shader&) = delete;

	shader(shader&& Shader) noexcept;
	shader& operator=(shader&& Shader) noexcept;

	void Compile(const str& Path);
	void Bind() const;

	void SetUniform(str_view Name, s32 V1) const;
	void SetUniform(str_view Name, float V1) const;
	void SetUniform(str_view Name, glm::vec3 V1) const;
	void SetUniform(str_view Name, const glm::mat4& Matrix) const;
	void SetUniform(str_view Name, span<glm::mat4> Matrix) const;
	void SetUniform(str_view Name, const glm::mat3& Matrix) const;
	void SetUniform(str_view Name, const phong_material& Material) const;
	void SetUniform(str_view Name, const light& Light, const glm::mat4& View, s32 Index = -1) const;
	void SetUniform(str_view Name, str_view CountName, span<light> Lights, const glm::mat4& View) const;

	// TODO: completely remove strings from hot path, do it preemptively and cache actual uniform ids
	// think about that after renderer refactor and shader tangent space refactor
	template <typename input_type>
	void MaybeSetUniform(const input_type& Value, str_view First, s32 Index = -1, str_view Second = {}) const {
		// Hot path:
		using value_type = std::remove_const<input_type>::type;
		const auto Hash = uniform_identifier::hasher::Hash(First, Index, Second);
		if (auto* Existing =
				UniformsCache.GetCache<value_type>().FindByPredicate(Hash, [First, Index, Second](auto& Identifier) {
					return Identifier.Key.SecondName == Second && Identifier.Key.Index == Index &&
						   Identifier.Key.FirstName == First;
				})) {
			if (Existing->Value.Value == Value) {
				return;
			} else {
				Existing->Value.Value = Value;
				uniforms_cache::SetValue(Existing->Value.ResourceId, Value);
				return;
			}
		}
		// Cold path:
		str FirstString{First};
		str FullName = FirstString;
		if (Index != -1) {
			FullName += '[' + str_util::FromInt(Index) + ']';
		}
		str SecondString{Second};
		if (!Second.IsEmpty()) {
			FullName += '.' + SecondString;
		}
		const auto Location = GetUniformLocation(FullName);
		uniforms_cache::SetValue(Location, Value);
		uniforms_cache::cached_uniform<value_type> NewUniform{Location, Value};
		UniformsCache
			.GetCache<value_type>()[uniform_identifier{std::move(FirstString), Index, std::move(SecondString)}] =
			NewUniform;
	}

	template <typename value_type>
	FORCEINLINE void MaybeSetUniform(const value_type& Value, str_view First, str_view Second) const {
		return MaybeSetUniform(Value, First, -1, Second);
	}

private:
	struct parsed_shaders {
		str VertexShader{};
		str GeometryShader{};
		str FragmentShader{};
	};

	s32 GetUniformLocation(str_view FullName) const;
	static parsed_shaders ParseShader(const str& Path);
	static u32 CompileShader(u32 Type, const str& Source);
};
