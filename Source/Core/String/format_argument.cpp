#include "format_argument.h"
#include "str_conversions.h"
#include "str_format.h"

namespace strings {
index format_argument::GetCharSize() const {
	switch (Type) {
		case type::string_arg:
			return String.GetSize();
		case type::bool_arg:
			return default_bool_format::GetCharSize(Bool);
		case type::unsigned_integer_arg:
			return default_int_format<u64>::GetCharSize(UnsignedInteger);
		case type::signed_integer_arg:
			return default_int_format<s64>::GetCharSize(SignedInteger);
		case type::float_arg:
			return default_float_format::GetCharSize(Float);
		case type::pointer_arg:
			return default_pointer_format::GetCharSize(Pointer);
		case type::empty:
			CHECK(false);
			break;
	}
}

void format_argument::Write(mutable_str_view Destination) const {
	switch (Type) {
		case type::string_arg:
			std::memcpy(Destination.GetData(), String.GetData(), Destination.GetSize());
			break;
		case type::bool_arg:
			default_bool_format::Write(Destination, Bool);
			break;
		case type::unsigned_integer_arg:
			default_int_format<u64>::Write(Destination, UnsignedInteger);
			break;
		case type::signed_integer_arg:
			default_int_format<s64>::Write(Destination, SignedInteger);
			break;
		case type::float_arg:
			default_float_format::Write(Destination, Float);
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