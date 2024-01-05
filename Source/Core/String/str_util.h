#pragma once

#include "basic.h"
#include "str.h"

namespace str_util {

FORCEINLINE constexpr static index GetByteLength(str_view String);

FORCEINLINE constexpr bool IsSpace(char Character);
FORCEINLINE constexpr bool IsNumber(char Character);

template <integral integral_type>
static str FromInt(integral_type IntegralNumber);
template <fractional float_type>
static str FromFloat(float_type FloatingNumber, index FractionalPrecision = 4);

template <integral integral_type>
constexpr static integral_type GetNumber(str_view String);
template <fractional float_type>
constexpr static float_type GetNumber(str_view String);

FORCEINLINE constexpr str_view GetSubstring(str_view Source, index Begin, index End);

constexpr str_view GetIntegerString(str_view String);
constexpr str_view EatSpaces(str_view String);
constexpr str_view GetLine(str_view String, str_view Previous = {});


FORCEINLINE constexpr bool StartsWith(str_view String, str_view Substring);
FORCEINLINE constexpr index FindLastOf(str_view String, char Char);
FORCEINLINE constexpr index FindSubstring(str_view String, str_view Substring, index StartIndex = 0);

constexpr index GetByteLength(str_view String) {
	return String.GetSize();
}

constexpr str_view GetIntegerString(str_view String) {
	//		str_view Start = EatSpaces(String);
	//		index NumberLength = 0;
	//		while (NumberLength < Start.Size() && )
	return {};
}

constexpr bool IsSpace(char Character) {
	return Character == ' ' || Character == '\t' || Character == '\n' || Character == '\r';
}

constexpr bool IsNumber(char Character) {
	return '0' <= Character && Character <= '9';
}

constexpr str_view EatSpaces(str_view String) {
	if (String.IsEmpty()) {
		return {};
	}
	const char* NewStart = String.GetData();
	const char* StringEnd = String.GetData() + String.GetSize();
	while (IsSpace(*NewStart) && NewStart < StringEnd) {
		++NewStart;
	}
	if (NewStart == StringEnd) {
		return {};
	}
	return str_view{NewStart, StringEnd};
}

constexpr str_view GetLine(str_view String, str_view Previous) {
	if (String.IsEmpty()) {
		return {};
	}
	const char* LineStart = String.GetData();
	if (Previous.GetData() != nullptr) {
		LineStart = Previous.GetData() + Previous.GetSize();
		if (*LineStart == '\n') {
			++LineStart;
		} else {
			return {};
		}
	}
	const char* LineEnd = LineStart;
	const char* StringEnd = String.GetData() + GetByteLength(String);
	while (*LineEnd != '\n' && LineEnd < StringEnd) {
		++LineEnd;
	}
	return str_view{LineStart, LineEnd};
}

constexpr str_view GetSubstring(str_view Source, index Begin, index End) {
	return str_view{Source.GetData() + Begin, End - Begin};
}

constexpr bool StartsWith(str_view String, str_view Substring) {
	for (s32 Index = 0; Index < Substring.GetSize(); ++Index) {
		const bool TooSmall = Index >= String.GetSize();
		const bool Different = String[Index] != Substring[Index];
		if (TooSmall || Different) {
			return false;
		}
	}
	return true;
}

constexpr index FindLastOf(str_view String, char Char) {
	for (s32 Index = GetByteLength(String); Index >= 0; --Index) {
		if (String[Index] == Char) {
			return Index;
		}
	}
	return InvalidIndex;
}

constexpr index FindSubstring(str_view String, str_view Substring, index StartIndex) {
	if (Substring.IsEmpty() || String.GetSize() <= StartIndex) {
		return InvalidIndex;
	}
	span Tail = String;
	while (!Tail.IsEmpty()) {
		if (StartsWith(Tail, Substring)) {
			return String.GetSize() - Tail.GetSize();
		}
		Tail = Tail.RemoveFirst();
	}
	return InvalidIndex;
}

template <fractional float_type>
constexpr float_type GetNumber(str_view String) {
	// TODO
	return 0.f;
}

template <integral integral_type>
constexpr integral_type GetNumber(str_view String) {
	// TODO
	return 0;
}

template <fractional float_type>
str FromFloat(float_type FloatingNumber, index FractionalPrecision) {
	return {};	  // TODO
}

template <integral integral_type>
str FromInt(integral_type IntegralNumber) {
	// clang-format off
	constexpr array<char, 19> LookupTable =
		{'9','8','7','6','5','4','3','2','1','0','1','2','3','4','5','6','7','8','9'};
	constexpr index ZeroIndex = 9;
	array<char, 20> Buffer{}; // Should be enough for negative 64 bit int
	// clang-format on
	char* NumberStart = Buffer.end();
	bool Signed = IntegralNumber < 0;
	index NumChars = Signed ? 1 : 0;
	do {
		*--NumberStart = LookupTable[ZeroIndex + (IntegralNumber % 10)];
		IntegralNumber /= 10;
		++NumChars;
	} while (IntegralNumber);
	if (Signed) {
		*--NumberStart = '-';
	}

	return str{NumberStart, NumChars};
}

}	 // namespace str_util