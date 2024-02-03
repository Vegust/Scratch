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

	type Type;
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
		void* Pointer;
	};

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
	void Write(char* Destination) const;
};

template <index NumArguments>
struct format_argument_array {
	const format_argument Data[NumArguments];

	template <typename... argument_types>
	constexpr explicit format_argument_array(const argument_types&... Arguments) : Data{Arguments...} {
	}

	constexpr const format_argument& operator[](index ArgIndex) const {
		return Data[ArgIndex];
	}
};

template <index NumArguments>
inline index GetFormatLength(str_view String, const format_argument_array<NumArguments>& Arguments) {
	str_view StringTail{String};
	index PlacedArguments{0};
	index Length{0};
	index OpeningIndex = FindFirstOf(StringTail, '{');
	while (OpeningIndex != InvalidIndex && PlacedArguments < NumArguments) {
		const str_view RestOfString = strings::GetSubstring(StringTail, OpeningIndex + 1, StringTail.GetSize());
		const index ClosingIndex = FindFirstOf(RestOfString, '}');
		if (ClosingIndex == InvalidIndex) {
			break;
		}
		Length += OpeningIndex;
		Length += Arguments[PlacedArguments].GetCharSize();
		++PlacedArguments;
		StringTail = strings::GetSubstring(StringTail, OpeningIndex + 1 + ClosingIndex + 1, StringTail.GetSize());
		OpeningIndex = FindFirstOf(StringTail, '{');
	}
	Length += strings::GetByteLength(StringTail);
	return Length;
}

template <index NumArguments>
inline index WriteFormat(char* Destination, str_view String, const format_argument_array<NumArguments>& Arguments) {
	char* WritePosition = Destination;
	str_view StringTail{String};
	index PlacedArguments{0};
	index Length{0};
	index OpeningIndex = FindFirstOf(StringTail, '{');
	while (OpeningIndex != InvalidIndex && PlacedArguments < NumArguments) {
		const str_view RestOfString = strings::GetSubstring(StringTail, OpeningIndex + 1, StringTail.GetSize());
		const index ClosingIndex = FindFirstOf(RestOfString, '}');
		if (ClosingIndex == InvalidIndex) {
			break;
		}
		std::memcpy(WritePosition, StringTail.GetData(), OpeningIndex);
		Length += OpeningIndex;
		WritePosition += OpeningIndex;
		const index ArgumentSize = Arguments[PlacedArguments].GetCharSize();
		Arguments[PlacedArguments].Write(WritePosition);
		Length += ArgumentSize;
		WritePosition += ArgumentSize;
		++PlacedArguments;
		StringTail = strings::GetSubstring(StringTail, OpeningIndex + 1 + ClosingIndex + 1, StringTail.GetSize());
		OpeningIndex = FindFirstOf(StringTail, '{');
	}
	Length += strings::GetByteLength(StringTail);
	std::memcpy(WritePosition, StringTail.GetData(), StringTail.GetSize());
	return Length;
}

template <index NumArguments>
str Format(str_view String, const format_argument_array<NumArguments>& Arguments) {
	index FormatLength = GetFormatLength(String, Arguments);
	str Result{};
	Result.Reserve(FormatLength + 1);
	Result.OverwriteSize(FormatLength + 1);
	WriteFormat(Result.GetData(), String, Arguments);
	Result[FormatLength] = 0;
	return Result;
}

template <typename... argument_types>
str Format(str_view String, const argument_types&... Arguments) {
	constexpr index NumArguments = sizeof...(Arguments);
	format_argument_array<NumArguments> ArgArray{Arguments...};
	return Format(String, ArgArray);
}

template <typename... argument_types>
inline index GetFormatLength(str_view String, const argument_types&... Arguments) {
	constexpr index NumArguments = sizeof...(Arguments);
	format_argument_array<NumArguments> ArgArray{Arguments...};
	return GetFormatLength(String, ArgArray);
}

template <typename... argument_types>
inline index WriteFormat(char* Destination, str_view String, const argument_types&... Arguments) {
	constexpr index NumArguments = sizeof...(Arguments);
	format_argument_array<NumArguments> ArgArray{Arguments...};
	return WriteFormat(Destination, String, ArgArray);
}

}	 // namespace strings
