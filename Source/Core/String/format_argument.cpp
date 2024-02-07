#include "format_argument.h"
#include "str_conversions.h"

namespace strings {
index format_argument::GetCharSize() const {
	if (CachedData.Cached) {
		return CachedData.CharSize;
	}
	CachedData.Cached = true;
	switch (Type) {
		case type::string_arg:
			CachedData.CharSize = String.GetSize();
			break;
		case type::bool_arg:
			CachedData.CharSize = default_bool_format::GetCharSize(Bool);
			break;
		case type::u8_arg:
			CachedData.CharSize = default_int_format<u8>::GetCharSize(U8);
			break;
		case type::u16_arg:
			CachedData.CharSize = default_int_format<u16>::GetCharSize(U16);
			break;
		case type::u32_arg:
			CachedData.CharSize = default_int_format<u32>::GetCharSize(U32);
			break;
		case type::u64_arg:
			CachedData.CharSize = default_int_format<u64>::GetCharSize(U64);
			break;
		case type::s8_arg:
			CachedData.CharSize = default_int_format<s8>::GetCharSize(S8);
			break;
		case type::s16_arg:
			CachedData.CharSize = default_int_format<s16>::GetCharSize(S16);
			break;
		case type::s32_arg:
			CachedData.CharSize = default_int_format<s32>::GetCharSize(S32);
			break;
		case type::s64_arg:
			CachedData.CharSize = default_int_format<s64>::GetCharSize(S64);
			break;
		case type::float_arg:
			CachedData.DecimalParts = math::FloatToDecimal(Float);
			CachedData.CharSize = default_float_format::GetCharSize(CachedData.DecimalParts);
			break;
		case type::double_arg:
			CachedData.DecimalParts = math::FloatToDecimal(Double);
			CachedData.CharSize = default_float_format::GetCharSize(CachedData.DecimalParts);
			break;
		case type::pointer_arg:
			CachedData.CharSize = default_pointer_format::GetCharSize(Pointer);
			break;
		case type::empty:
			CHECK(false);
			break;
	}
	return CachedData.CharSize;
}

void format_argument::Write(mutable_str_view Destination) const {
	index Size = GetCharSize();
	switch (Type) {
		case type::string_arg:
			std::memcpy(Destination.GetData(), String.GetData(), Size);
			break;
		case type::bool_arg:
			default_bool_format::Write(Destination, Bool);
			break;
		case type::u8_arg:
			default_int_format<u8>::Write(Destination, U8);
			break;
		case type::u16_arg:
			default_int_format<u16>::Write(Destination, U16);
			break;
		case type::u32_arg:
			default_int_format<u32>::Write(Destination, U32);
			break;
		case type::u64_arg:
			default_int_format<u64>::Write(Destination, U64);
			break;
		case type::s8_arg:
			default_int_format<s8>::Write(Destination, S8);
			break;
		case type::s16_arg:
			default_int_format<s16>::Write(Destination, S16);
			break;
		case type::s32_arg:
			default_int_format<s32>::Write(Destination, S32);
			break;
		case type::s64_arg:
			default_int_format<s64>::Write(Destination, S64);
			break;
		case type::float_arg:
			default_float_format::Write(Destination, CachedData.DecimalParts);
			break;
		case type::double_arg:
			default_float_format::Write(Destination, CachedData.DecimalParts);
			break;
		case type::pointer_arg:
			default_pointer_format::Write(Destination, Pointer);
			break;
		case type::empty:
			CHECK(false);
			break;
	}
}
}	 // namespace strings