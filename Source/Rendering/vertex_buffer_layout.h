//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"
SCRATCH_DISABLE_WARNINGS_BEGIN();
#include "glad/glad.h"
SCRATCH_DISABLE_WARNINGS_END();

#include <vector>

struct vertex_buffer_attribute
{
	uint32 Type{0};
	uint32 Count{0};
	uint8 Normalized{0};

	static uint32 GetSizeOfType(uint32 Type)
	{
		switch (Type)
		{
			case GL_FLOAT:
				return sizeof(float);
			case GL_UNSIGNED_INT:
				return sizeof(uint32);
			case GL_UNSIGNED_BYTE:
				return sizeof(uint8);
			default:
				std::unreachable();
		}
	}
};

template <typename T>
concept buffer_attribute_type = std::is_floating_point<T>::value || std::is_integral<T>::value;

struct vertex_buffer_layout
{
	std::vector<vertex_buffer_attribute> Attributes{};
	uint32 Stride{0};

	[[nodiscard]] const std::vector<vertex_buffer_attribute>& GetAttributes() const
	{
		return Attributes;
	}

	[[nodiscard]] uint32 GetStride() const
	{
		return Stride;
	}

	template <buffer_attribute_type T>
	void Push(uint32 Count)
	{
		Stride += sizeof(T) * Count;
		if constexpr (std::is_same<T, float>::value)
		{
			Attributes.emplace_back(GL_FLOAT, Count, GL_FALSE);
			return;
		}
		else if constexpr (std::is_same<T, uint32>::value)
		{
			Attributes.emplace_back(GL_UNSIGNED_INT, Count, GL_FALSE);
			return;
		}
		else if constexpr (std::is_same<T, uint8>::value)
		{
			Attributes.emplace_back(GL_UNSIGNED_BYTE, Count, GL_TRUE);
			return;
		}
		std::unreachable();
	}
};
