#pragma once

#include "core_types.h"
#include "SceneObjects/light.h"
#include "SceneObjects/phong_material.h"
#include "Containers/span.h"
#include "glm/fwd.hpp"

class shader {
public:
	str mPath;
	u32 mRendererId{0};

	struct uniform_identifier {
		// First + Index + Second = full name string.
		// (e.g. Lights + 2 + Attenuation = Lights[2].Attenuation).
		// Stored separately, so we don't need to do string allocations when
		// setting uniform values
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

	mutable struct uniforms_cache {
		template <typename value_type>
		struct cached_uniform {
			s32 mResourceId{0};
			value_type mValue{};
		};

		hash_table<uniform_identifier, cached_uniform<s32>> mInts;
		hash_table<uniform_identifier, cached_uniform<float>> mFloats;
		hash_table<uniform_identifier, cached_uniform<glm::vec3>> mVec3s;
		hash_table<uniform_identifier, cached_uniform<glm::vec4>> mVec4s;
		hash_table<uniform_identifier, cached_uniform<glm::mat3>> mMat3s;
		hash_table<uniform_identifier, cached_uniform<glm::mat4>> mMat4s;

		template <typename value_type>
		auto& GetCache() {
			if constexpr (std::is_same_v<value_type, s32>) {
				return mInts;
			} else if constexpr (std::is_same_v<value_type, float>) {
				return mFloats;
			} else if constexpr (std::is_same_v<value_type, glm::vec3>) {
				return mVec3s;
			} else if constexpr (std::is_same_v<value_type, glm::vec4>) {
				return mVec4s;
			} else if constexpr (std::is_same_v<value_type, glm::mat3>) {
				return mMat3s;
			} else if constexpr (std::is_same_v<value_type, glm::mat4>) {
				return mMat4s;
			};
		}

		static void SetValue(s32 ResourceId, s32 Value);
		static void SetValue(s32 ResourceId, float Value);
		static void SetValue(s32 ResourceId, const glm::vec3& Value);
		static void SetValue(s32 ResourceId, const glm::vec4& Value);
		static void SetValue(s32 ResourceId, const glm::mat3& Value);
		static void SetValue(s32 ResourceId, const glm::mat4& Value);

		void Clear() {
			mInts.Clear();
			mFloats.Clear();
			mVec3s.Clear();
			mVec4s.Clear();
			mMat3s.Clear();
			mMat4s.Clear();
		}

	} mUniformsCache;

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
	void SetUniform(str_view Name, str_view CountName, span<light> Lights, const glm::mat4& View)
		const;

	template <typename input_type>
	void MaybeSetUniform(const input_type& Value, str_view First, s32 Index = -1, str_view Second = {})
		const {
		using value_type = std::remove_const<input_type>::type;
		const auto Hash = uniform_identifier::hasher::Hash(First, Index, Second);
		if (auto* Existing = mUniformsCache.GetCache<value_type>().FindByPredicate(
				Hash, [First, Index, Second](auto& Identifier) {
					return Identifier.mKey.mSecondName == Second && Identifier.mKey.mIndex == Index &&
						   Identifier.mKey.mFirstName == First;
				})) {
			if (Existing->mValue.mValue == Value) {
				return;
			} else {
				Existing->mValue.mValue = Value;
				uniforms_cache::SetValue(Existing->mValue.mResourceId, Value);
				return;
			}
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
		const auto Location = GetUniformLocation(FullName);
		uniforms_cache::SetValue(Location, Value);
		uniforms_cache::cached_uniform<value_type> NewUniform{Location, Value};
		mUniformsCache.GetCache<value_type>()[uniform_identifier{
			std::move(FirstString), Index, std::move(SecondString)}] = NewUniform;
	}

	template <typename value_type>
	void MaybeSetUniform(const value_type& Value, str_view First, str_view Second) const {
		return MaybeSetUniform(Value, First, -1, Second);
	}

private:
	struct parsed_shaders {
		str mVertexShader{};
		str mGeometryShader{};
		str mFragmentShader{};
	};

	s32 GetUniformLocation(str_view FullName) const;
	static parsed_shaders ParseShader(const str& Path);
	static u32 CompileShader(u32 Type, const str& Source);
};
