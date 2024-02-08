#pragma once

#include "str.h"
#include "str_conversions.h"
#include "atom.h"
#include "format_argument.h"

namespace strings {

constexpr index GetFormatLength(str_view String, span<format_argument> Arguments) {
	str_view StringTail{String};
	index PlacedArguments{0};
	index Length{0};
	index OpeningIndex = FindFirstOf(StringTail, '{');
	while (OpeningIndex != InvalidIndex && PlacedArguments < Arguments.GetSize()) {
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

constexpr index WriteFormat(mutable_str_view Destination, str_view String, span<format_argument> Arguments) {
	char* WritePosition = Destination.GetData();
	str_view StringTail{String};
	index PlacedArguments{0};
	index Length{0};
	index OpeningIndex = FindFirstOf(StringTail, '{');
	while (OpeningIndex != InvalidIndex && PlacedArguments < Arguments.GetSize()) {
		const str_view RestOfString = strings::GetSubstring(StringTail, OpeningIndex + 1, StringTail.GetSize());
		const index ClosingIndex = FindFirstOf(RestOfString, '}');
		if (ClosingIndex == InvalidIndex) {
			break;
		}
		CopyChars(WritePosition, StringTail.GetData(), OpeningIndex);
		Length += OpeningIndex;
		WritePosition += OpeningIndex;
		const index ArgumentSize = Arguments[PlacedArguments].GetCharSize();
		Arguments[PlacedArguments].Write(mutable_str_view{WritePosition, ArgumentSize});
		Length += ArgumentSize;
		WritePosition += ArgumentSize;
		++PlacedArguments;
		StringTail = strings::GetSubstring(StringTail, OpeningIndex + 1 + ClosingIndex + 1, StringTail.GetSize());
		OpeningIndex = FindFirstOf(StringTail, '{');
		CHECK(Destination.GetSize() >= Length);
	}
	Length += strings::GetByteLength(StringTail);
	CHECK(Destination.GetSize() >= Length);
	CopyChars(WritePosition, StringTail.GetData(), StringTail.GetSize());
	return Length;
}

FORCEINLINE str Format(str_view String, span<format_argument> Arguments) {
	index FormatLength = GetFormatLength(String, Arguments);
	str Result{};
	WriteFormat(Result.AppendUninitialized(FormatLength), String, Arguments);
	Result[FormatLength] = 0;
	return Result;
}

template <format_supported... argument_types>
FORCEINLINE str Format(str_view String, const argument_types&... Arguments) {
	array<format_argument, sizeof...(Arguments)> ArgumentArray{Arguments...};
	return Format(String, ArgumentArray);
}

template <format_supported... argument_types>
FORCEINLINE constexpr index GetFormatLength(str_view String, const argument_types&... Arguments) {
	array<format_argument, sizeof...(Arguments)> ArgumentArray{Arguments...};
	return GetFormatLength(String, ArgumentArray);
}

template <format_supported... argument_types>
FORCEINLINE constexpr index WriteFormat(
	mutable_str_view Destination,
	str_view String,
	const argument_types&... Arguments) {
	array<format_argument, sizeof...(Arguments)> ArgumentArray{Arguments...};
	return WriteFormat(Destination, String, ArgumentArray);
}

}	 // namespace strings
