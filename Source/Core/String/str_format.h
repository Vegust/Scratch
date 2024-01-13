#pragma once

#include "str.h"
#include "str_conversions.h"
#include "atom.h"

namespace strings {

struct format_argument {
	enum class type {
		string_arg,
		bool_arg,
		u8_arg,
		u16_arg,
		u32_arg,
		u64_arg,
		s8_arg,
		s16_arg,
		s32_arg,
		s64_arg,
		float_arg,
		double_arg,
		pointer_arg
	} Type;

	union {
		str_view String;
		bool Bool;
		u8 U8;
		u16 U16;
		u32 U32;
		u64 U64;
		s8 S8;
		s16 S16;
		s32 S32;
		s64 S64;
		float Float;
		double Double;
		void* Pointer;
	};

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

	FORCEINLINE constexpr format_argument(u8 Val) : U8{Val}, Type{type::u8_arg} {
	}

	FORCEINLINE constexpr format_argument(u16 Val) : U16{Val}, Type{type::u16_arg} {
	}

	FORCEINLINE constexpr format_argument(u32 Val) : U32{Val}, Type{type::u32_arg} {
	}

	FORCEINLINE constexpr format_argument(u64 Val) : U64{Val}, Type{type::u64_arg} {
	}

	FORCEINLINE constexpr format_argument(s8 Val) : S8{Val}, Type{type::s8_arg} {
	}

	FORCEINLINE constexpr format_argument(s16 Val) : S16{Val}, Type{type::s16_arg} {
	}

	FORCEINLINE constexpr format_argument(s32 Val) : S32{Val}, Type{type::s32_arg} {
	}

	FORCEINLINE constexpr format_argument(s64 Val) : S64{Val}, Type{type::s64_arg} {
	}

	FORCEINLINE constexpr format_argument(float Val) : Float{Val}, Type{type::float_arg} {
	}

	FORCEINLINE constexpr format_argument(double Val) : Double{Val}, Type{type::double_arg} {
	}

	FORCEINLINE constexpr format_argument(void* Val) : Pointer{Val}, Type{type::pointer_arg} {
	}
};

template <u64 NumArguments>
struct format_argument_array {
	format_argument Data[NumArguments];

	template <typename... argument_types>
	format_argument_array(const argument_types&... Arguments) : Data{Arguments...} {
	}
};

template <typename... argument_types>
str Format(str_view String, const argument_types&... Arguments) {
	constexpr u64 NumArguments = sizeof...(Arguments);
	format_argument_array<NumArguments> ArgArray{Arguments...};
	return Format(String, ArgArray);
}

template <u64 NumArguments>
str Format(str_view String, const format_argument_array<NumArguments>& Arguments) {
	return str{String};
}

}	 // namespace strings
