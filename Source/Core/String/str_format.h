#pragma once

#include "str.h"
#include "str_conversions.h"

namespace strings {

struct format_argument {
	enum class type { string_arg, uint_arg, int_arg, float_arg, double_arg, pointer_arg } Type;

	union {
		str_view String;
		u64 UInt;
		s64 Int;
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
	
	FORCEINLINE constexpr format_argument(bool Val) : UInt{Val}, Type{type::uint_arg} {
	}
	
	FORCEINLINE constexpr format_argument(u8 Val) : UInt{Val}, Type{type::uint_arg} {
	}
	
	FORCEINLINE constexpr format_argument(s8 Val) : Int{Val}, Type{type::int_arg} {
	}

	FORCEINLINE constexpr format_argument(u16 Val) : UInt{Val}, Type{type::uint_arg} {
	}

	FORCEINLINE constexpr format_argument(s16 Val) : Int{Val}, Type{type::int_arg} {
	}
	
	FORCEINLINE constexpr format_argument(u32 Val) : UInt{Val}, Type{type::uint_arg} {
	}
	
	FORCEINLINE constexpr format_argument(s32 Val) : Int{Val}, Type{type::int_arg} {
	}
	
	FORCEINLINE constexpr format_argument(u64 Val) : UInt{Val}, Type{type::uint_arg} {
	}
	
	FORCEINLINE constexpr format_argument(s64 Val) : Int{Val}, Type{type::int_arg} {
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