#include "str_format.h"

namespace strings {

index GetFormatLength(str_view String, span<format_argument> Arguments) {
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

index WriteFormat(char* Destination, str_view String, span<format_argument> Arguments) {
	char* WritePosition = Destination;
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

str Format(str_view String, span<format_argument> Arguments) {
	index FormatLength = GetFormatLength(String, Arguments);
	str Result{};
	Result.Reserve(FormatLength + 1);
	Result.OverwriteSize(FormatLength + 1);
	WriteFormat(Result.GetData(), String, Arguments);
	Result[FormatLength] = 0;
	return Result;
}
}	 // namespace strings