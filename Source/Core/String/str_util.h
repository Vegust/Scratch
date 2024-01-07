#pragma once

#include "basic.h"
#include "Templates/result.h"
#include "str.h"

namespace str_util {

FORCEINLINE constexpr static index GetByteLength(str_view String);

FORCEINLINE constexpr bool IsSpace(char Character);
FORCEINLINE constexpr bool IsNumber(char Character);
FORCEINLINE constexpr bool IsSign(char Character);

template <integral integral_type>
static str FromInt(integral_type IntegralNumber);
template <fractional float_type>
static str FromFloat(float_type FloatingNumber);

template <numeric numeric_type>
constexpr static numeric_type GetNumberChecked(str_view String);
template <integral integral_type>
constexpr static result<integral_type> GetNumber(str_view String);
template <fractional float_type>
constexpr static result<float_type> GetNumber(str_view String);

FORCEINLINE constexpr str_view GetSubstring(str_view Source, index Begin, index End);

constexpr result<str_view> GetIntegerString(str_view String);
constexpr str_view EatSpaces(str_view String);
constexpr str_view GetLine(str_view String, str_view Previous = {});

FORCEINLINE constexpr bool StartsWith(str_view String, str_view Substring);
FORCEINLINE constexpr index FindLastOf(str_view String, char Char);
FORCEINLINE constexpr index FindSubstring(str_view String, str_view Substring, index StartIndex = 0);

constexpr index GetByteLength(str_view String) {
	return String.GetSize();
}

constexpr result<str_view> GetIntegerString(str_view String) {
	str_view Start = EatSpaces(String);
	index Length = 0;
	bool Signed = false;
	if (!Start.IsEmpty() && IsSign(Start[0])) {
		Signed = true;
		++Length;
	}
	while (Length < Start.GetSize() && IsNumber(Start[Length])) {
		++Length;
	}
	if (Length == static_cast<index>(Signed)) {
		return common_errors::invalid_input;
	} else {
		return str_view{Start.GetData(), Length};
	}
}

constexpr bool IsSpace(char Character) {
	return Character == ' ' || Character == '\t' || Character == '\n' || Character == '\r';
}

constexpr bool IsNumber(char Character) {
	return '0' <= Character && Character <= '9';
}

constexpr bool IsSign(char Character) {
	return Character == '-' || Character == '+';
}

constexpr str_view EatSpaces(str_view String) {
	str_view Result = String;
	while (!Result.IsEmpty() && IsSpace(Result[0])) {
		Result = Result.RemoveFirst();
	}
	return Result;
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

template <numeric numeric_type>
constexpr numeric_type GetNumberChecked(str_view String) {
	return GetNumber<numeric_type>(String).GetValue();
}

template <integral integral_type>
constexpr result<integral_type> GetNumber(str_view String) {
	if (auto Result = GetIntegerString(String)) {
		str_view IntegerString = Result.GetValue();
		integral_type Sign = 1;
		if (IsSign(IntegerString[0])) {
			if constexpr (signed_integral<integral_type>) {
				Sign = IntegerString[0] == '-' ? -1 : 1;
			} else {
				if (IntegerString[0] == '-') {
					return common_errors::invalid_sign;
				}
			}
			IntegerString = IntegerString.RemoveFirst();
		}
		integral_type Number{0};
		while (!IntegerString.IsEmpty()) {
			const integral_type Digit = IntegerString[0] - '0';
			constexpr s32 MaxSafe = std::numeric_limits<integral_type>::max();
			if (Number > ((MaxSafe - Digit) / 10)) {
				return common_errors::input_too_big;
			}
			Number = Number * 10 + Digit;
			IntegerString = IntegerString.RemoveFirst();
		}
		return Number * Sign;
	}
	return common_errors::invalid_input;
}

template <fractional float_type>
constexpr result<float_type> GetNumber(str_view String) {
	// TODO
	return 0.f;
}

template <integral integral_type>
str FromInt(integral_type IntegralNumber) {
	constexpr array<char, 10> LookupTable = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	array<char, 20> Buffer{};	 // Should be enough for negative 64 bit int
	char* NumberStart = Buffer.end();
	integral_type Value = math::Abs(IntegralNumber);
	index NumChars = 0;
	do {
		*--NumberStart = LookupTable[Value % 10];
		++NumChars;
		Value /= 10;
	} while (Value);
	if (IntegralNumber < 0) {
		*--NumberStart = '-';
		++NumChars;
	}
	return str{NumberStart, NumChars};
}

template <fractional float_type>
str FromFloat(float_type FloatingNumber) {
	const math::decimal_parts Parts = math::FloatToDecimal(FloatingNumber);
	if (Parts.IsNaN) {
		return str{"NaN"};
	}
	if (Parts.IsInfinity) {
		return str{Parts.IsNegative ? "-inf" : "inf"};
	}
	if (Parts.Significand == 0) {
		return str("0");
	}
	constexpr array<char, 10> LookupTable = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	array<char, 23> Buffer{};	 // '-' + 17 digits significand + E + '-' + 3 digits exponent
	char* NumberStart = Buffer.end();
	const s32 NumSignificandDigits = math::GetNumDigits(Parts.Significand);
	s32 ExponentValue = math::Abs(Parts.Exponent + NumSignificandDigits - 1);
	index NumChars = 0;
	do {
		*--NumberStart = LookupTable[ExponentValue % 10];
		ExponentValue /= 10;
		++NumChars;
	} while (ExponentValue);
	if ((Parts.Exponent + NumSignificandDigits - 1) < 0) {
		*--NumberStart = '-';
		++NumChars;
	}
	*--NumberStart = 'e';
	++NumChars;
	s32 SignificandValue = math::Abs(Parts.Significand);
	do {
		*--NumberStart = LookupTable[SignificandValue % 10];
		++NumChars;
		SignificandValue /= 10;
		if (SignificandValue && SignificandValue < 10) {
			*--NumberStart = '.';
			++NumChars;
		}
	} while (SignificandValue);
	if (Parts.IsNegative) {
		*--NumberStart = '-';
		++NumChars;
	}
	return str{NumberStart, NumChars};
}

}	 // namespace str_util