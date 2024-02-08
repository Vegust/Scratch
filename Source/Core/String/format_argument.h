#pragma once

#include "basic.h"
#include "Templates/concepts.h"
#include "atom.h"
#include "str.h"
#include "str_conversions.h"

namespace strings {

struct format_argument;

template <typename type>
concept format_supported = numeric<type> || pointer<type> || std::convertible_to<type, str_view>;

struct format_argument {
	enum class type : u8 {
		empty,
		string_arg,
		bool_arg,
		signed_integer_arg,
		unsigned_integer_arg,
		float_arg,
		pointer_arg,
	};

	union {
		str_view String;
		bool Bool;
		s64 SignedInteger;
		u64 UnsignedInteger;
		math::decimal_parts Float;
		void* Pointer{nullptr};
	};

	type Type{type::empty};

	FORCEINLINE constexpr format_argument() = default;

	FORCEINLINE constexpr format_argument(const str& Val) : String{Val}, Type{type::string_arg} {
	}

	FORCEINLINE constexpr format_argument(str_view Val) : String{Val}, Type{type::string_arg} {
	}

	FORCEINLINE constexpr format_argument(const char* Val) : String{Val}, Type{type::string_arg} {
	}

	FORCEINLINE constexpr format_argument(atom Val) : String{Val.ToStr()}, Type{type::string_arg} {
	}

	FORCEINLINE constexpr format_argument(bool Val) : Bool{Val}, Type{type::bool_arg} {
	}

	FORCEINLINE constexpr format_argument(s8 Val) : SignedInteger{Val}, Type{type::signed_integer_arg} {
	}

	FORCEINLINE constexpr format_argument(u8 Val) : UnsignedInteger{Val}, Type{type::unsigned_integer_arg} {
	}

	FORCEINLINE constexpr format_argument(s16 Val) : SignedInteger{Val}, Type{type::signed_integer_arg} {
	}

	FORCEINLINE constexpr format_argument(u16 Val) : UnsignedInteger{Val}, Type{type::unsigned_integer_arg} {
	}

	FORCEINLINE constexpr format_argument(s32 Val) : SignedInteger{Val}, Type{type::signed_integer_arg} {
	}

	FORCEINLINE constexpr format_argument(u32 Val) : UnsignedInteger{Val}, Type{type::unsigned_integer_arg} {
	}

	FORCEINLINE constexpr format_argument(s64 Val) : SignedInteger{Val}, Type{type::signed_integer_arg} {
	}

	FORCEINLINE constexpr format_argument(u64 Val) : UnsignedInteger{Val}, Type{type::unsigned_integer_arg} {
	}

	FORCEINLINE constexpr format_argument(float Val) : Float{math::CalcFloatParts(Val)}, Type{type::float_arg} {
	}

	FORCEINLINE constexpr format_argument(double Val) : Float{math::CalcFloatParts(Val)}, Type{type::float_arg} {
	}

	FORCEINLINE constexpr format_argument(void* Val) : Pointer{Val}, Type{type::pointer_arg} {
	}

	FORCEINLINE constexpr index GetCharSize() const {
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
		std::unreachable();
	}

	FORCEINLINE constexpr void Write(mutable_str_view Destination) const {
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
};
}	 // namespace strings