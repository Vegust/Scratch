#include "str_format.h"

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
	}
	return CachedData.CharSize;
}

void format_argument::Write(char* Destination) const {
	index Size = GetCharSize();
	switch (Type) {
		case type::string_arg:
			std::memcpy(Destination, String.GetData(), Size);
			break;
		case type::bool_arg:
			default_bool_format::Write(Destination, Size, Bool);
			break;
		case type::u8_arg:
			default_int_format<u8>::Write(Destination, Size, U8);
			break;
		case type::u16_arg:
			default_int_format<u16>::Write(Destination, Size, U16);
			break;
		case type::u32_arg:
			default_int_format<u32>::Write(Destination, Size, U32);
			break;
		case type::u64_arg:
			default_int_format<u64>::Write(Destination, Size, U64);
			break;
		case type::s8_arg:
			default_int_format<s8>::Write(Destination, Size, S8);
			break;
		case type::s16_arg:
			default_int_format<s16>::Write(Destination, Size, S16);
			break;
		case type::s32_arg:
			default_int_format<s32>::Write(Destination, Size, S32);
			break;
		case type::s64_arg:
			default_int_format<s64>::Write(Destination, Size, S64);
			break;
		case type::float_arg:
			default_float_format::Write(Destination, Size, CachedData.DecimalParts);
			break;
		case type::double_arg:
			default_float_format::Write(Destination, Size, CachedData.DecimalParts);
			break;
		case type::pointer_arg:
			default_pointer_format::Write(Destination, Size, Pointer);
			break;
	}
}
}	 // namespace strings