#pragma once

#include "Containers/dyn_array.h"
#include "core_types.h"
#include "glad/glad.h"

struct vertex_buffer_attribute {
	u32 mType{0};
	u32 mCount{0};
	u8 mNormalized{0};

	static u32 GetSizeOfType(u32 Type) {
		switch (Type) {
			case GL_FLOAT:
				return sizeof(float);
			case GL_UNSIGNED_INT:
				return sizeof(u32);
			case GL_UNSIGNED_BYTE:
				return sizeof(u8);
			default:
				return 0;
		}
	}
};

template <typename T>
concept buffer_attribute_type = floating_point<T> || integral<T>;

struct vertex_buffer_layout {
	dyn_array<vertex_buffer_attribute> mAttributes{};
	u32 mStride{0};

	[[nodiscard]] const dyn_array<vertex_buffer_attribute>& GetAttributes() const {
		return mAttributes;
	}

	[[nodiscard]] u32 GetStride() const {
		return mStride;
	}

	template <buffer_attribute_type T>
	void Push(u32 Count) {
		mStride += sizeof(T) * Count;
		if constexpr (std::is_same<T, float>::value) {
			mAttributes.Emplace(GL_FLOAT, Count, GL_FALSE);
			return;
		} else if constexpr (std::is_same<T, u32>::value) {
			mAttributes.Emplace(GL_UNSIGNED_INT, Count, GL_FALSE);
			return;
		} else if constexpr (std::is_same<T, u8>::value) {
			mAttributes.Emplace(GL_UNSIGNED_BYTE, Count, GL_TRUE);
			return;
		}
	}
};
