#pragma once

#include "core.h"
#include "Containers/array.h"
#include "glad/glad.h"

struct vertex_buffer_attribute {
	u32 Type{0};
	u32 Count{0};
	u8 Normalized{0};

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

struct vertex_buffer_layout {
private:
	array<vertex_buffer_attribute, 4> Attributes{};
	u32 Size{0};
	u32 Stride{0};

public:
	[[nodiscard]] const array<vertex_buffer_attribute, 4>& GetAttributes() const {
		return Attributes;
	}

	[[nodiscard]] u32 GetSize() const {
		return Size;
	}

	[[nodiscard]] u32 GetStride() const {
		return Stride;
	}

	template <numeric T>
	void Push(u32 Count) {
		CHECK(Size < Attributes.GetSize())
		Stride += sizeof(T) * Count;
		if constexpr (std::is_same<T, float>::value) {
			Attributes[Size] = {GL_FLOAT, Count, GL_FALSE};
			++Size;
			return;
		} else if constexpr (std::is_same<T, u32>::value) {
			Attributes[Size] = {GL_UNSIGNED_INT, Count, GL_FALSE};
			++Size;
			return;
		} else if constexpr (std::is_same<T, u8>::value) {
			Attributes[Size] = {GL_UNSIGNED_BYTE, Count, GL_TRUE};
			++Size;
			return;
		}
	}
};
