#pragma once

#include "basic.h"
#include "Templates/concepts.h"
#include "atom.h"
#include "str.h"

namespace strings {
template <typename type>
concept format_supported = numeric<type> || pointer<type> || std::convertible_to<type, str_view>;

struct format_argument {
	enum class type {
		empty,
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
	};

	// I don't want to recalculate relatively heavy stuff multiple times
	// when I calculate length, print for same argument
	struct format_argument_cached_data {
		bool Cached{false};
		index CharSize{0};

		union {
			math::decimal_parts DecimalParts{};
		};
	};

	type Type{type::empty};
	mutable format_argument_cached_data CachedData{};

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
		void* Pointer{nullptr};
	};

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

	FORCEINLINE format_argument(u8 Val) : U8{Val}, Type{type::u8_arg} {
	}

	FORCEINLINE format_argument(u16 Val) : U16{Val}, Type{type::u16_arg} {
	}

	FORCEINLINE format_argument(u32 Val) : U32{Val}, Type{type::u32_arg} {
	}

	FORCEINLINE format_argument(u64 Val) : U64{Val}, Type{type::u64_arg} {
	}

	FORCEINLINE format_argument(s8 Val) : S8{Val}, Type{type::s8_arg} {
	}

	FORCEINLINE format_argument(s16 Val) : S16{Val}, Type{type::s16_arg} {
	}

	FORCEINLINE format_argument(s32 Val) : S32{Val}, Type{type::s32_arg} {
	}

	FORCEINLINE format_argument(s64 Val) : S64{Val}, Type{type::s64_arg} {
	}

	FORCEINLINE format_argument(float Val) : Float{Val}, Type{type::float_arg} {
	}

	FORCEINLINE format_argument(double Val) : Double{Val}, Type{type::double_arg} {
	}

	FORCEINLINE format_argument(void* Val) : Pointer{Val}, Type{type::pointer_arg} {
	}

	index GetCharSize() const;
	void Write(mutable_str_view Destination) const;
};
}	 // namespace strings