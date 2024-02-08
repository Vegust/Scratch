#pragma once

#include "basic.h"
#include "Templates/concepts.h"
#include "atom.h"
#include "str.h"

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

	FORCEINLINE format_argument() = default;

	FORCEINLINE format_argument(const str& Val) : String{Val}, Type{type::string_arg} {
	}

	FORCEINLINE format_argument(str_view Val) : String{Val}, Type{type::string_arg} {
	}

	FORCEINLINE format_argument(const char* Val) : String{Val}, Type{type::string_arg} {
	}

	FORCEINLINE format_argument(atom Val) : String{Val.ToStr()}, Type{type::string_arg} {
	}

	FORCEINLINE format_argument(bool Val) : Bool{Val}, Type{type::bool_arg} {
	}

	FORCEINLINE format_argument(s8 Val) : SignedInteger{Val}, Type{type::signed_integer_arg} {
	}

	FORCEINLINE format_argument(u8 Val) : UnsignedInteger{Val}, Type{type::unsigned_integer_arg} {
	}

	FORCEINLINE format_argument(s16 Val) : SignedInteger{Val}, Type{type::signed_integer_arg} {
	}

	FORCEINLINE format_argument(u16 Val) : UnsignedInteger{Val}, Type{type::unsigned_integer_arg} {
	}

	FORCEINLINE format_argument(s32 Val) : SignedInteger{Val}, Type{type::signed_integer_arg} {
	}

	FORCEINLINE format_argument(u32 Val) : UnsignedInteger{Val}, Type{type::unsigned_integer_arg} {
	}

	FORCEINLINE format_argument(s64 Val) : SignedInteger{Val}, Type{type::signed_integer_arg} {
	}

	FORCEINLINE format_argument(u64 Val) : UnsignedInteger{Val}, Type{type::unsigned_integer_arg} {
	}

	FORCEINLINE format_argument(float Val) : Float{math::decimal_parts(Val)}, Type{type::float_arg} {
	}

	FORCEINLINE format_argument(double Val) : Float{math::decimal_parts(Val)}, Type{type::float_arg} {
	}

	FORCEINLINE format_argument(void* Val) : Pointer{Val}, Type{type::pointer_arg} {
	}

	index GetCharSize() const;
	void Write(mutable_str_view Destination) const;
};
}	 // namespace strings