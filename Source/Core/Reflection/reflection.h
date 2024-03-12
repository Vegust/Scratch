#pragma once

#include "basic.h"
#include "Containers/array.h"

namespace reflection {

template <typename type>
concept supported = requires(const type& Value) { Value.GetFields(); };

enum class field_type {
	// simple:
	float_field,
	double_field,
	u8_field,
	u16_field,
	u32_field,
	u64_field,
	s8_field,
	s16_field,
	s32_field,
	s64_field,
	bool_field,
	// hard:
	string_field,
	enum_field,
	atom_field,
	array_field,
	hash_table_field,
	type_field
};

struct field {
	str_view Name;
	field_type Type;
	u64 ByteOffset;

	field() = default;
	field(str_view InName, field_type InType, u64 InOffset) : Name{InName}, Type{InType}, ByteOffset{InOffset} {};
};

template <typename type>
field_type GetFieldType();

template <>
field_type GetFieldType<s32>() {
	return field_type::s32_field;
}

template <>
field_type GetFieldType<float>() {
	return field_type::float_field;
}
}	 // namespace reflection