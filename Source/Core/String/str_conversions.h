#pragma once

#include "basic.h"
#include "Templates/result.h"
#include "str.h"

namespace strings {

FORCEINLINE constexpr static index GetByteLength(str_view String);

FORCEINLINE constexpr bool IsSpace(char Character);
FORCEINLINE constexpr bool IsNumber(char Character);
FORCEINLINE constexpr bool IsSign(char Character);

template <integral integral_type>
static str FromInt(integral_type IntegralNumber);
template <fractional float_type>
static str FromFloat(float_type FloatingNumber);
static str FromPointer(void* Pointer);

template <integral integral_type>
static void WriteInt(char* Destination, index NumberLength, integral_type IntegralNumber);
static void WriteFloat(char* Destination, index NumberLength, const math::decimal_parts& Parts);
static void WritePointer(char* Destination, index Length, void* Pointer);

template <numeric numeric_type>
constexpr static numeric_type GetNumberChecked(str_view String);
template <numeric numeric_type>
constexpr static result<numeric_type> GetNumber(str_view String);
template <integral integral_type>
result<integral_type> GetInteger(str_view String);
result<float> GetFloat(str_view String);
result<double> GetDouble(str_view String);

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

template <numeric numeric_type>
constexpr result<numeric_type> GetNumber(str_view String) {
	if constexpr (integral<numeric_type>) {
		return GetInteger<numeric_type>(String);
	} else if constexpr (sizeof(numeric_type) == 4) {
		return GetFloat(String);
	} else {
		return GetDouble(String);
	}
}

template <integral integral_type>
result<integral_type> GetInteger(str_view String) {
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

template <integral integral_type>
str FromInt(integral_type IntegralNumber) {
	const index NumberLength = math::GetNumDigits(IntegralNumber);
	str Result;
	Result.Reserve(NumberLength + 1);
	WriteInt(Result.GetData(), NumberLength, IntegralNumber);
	Result[NumberLength] = 0;
	Result.OverwriteSize(NumberLength + 1);
	return Result;
}

template <integral integral_type>
void WriteInt(char* Destination, index NumberLength, integral_type IntegralNumber) {
	const bool MinusSign = IntegralNumber < 0;
	char* NumberStart = Destination + NumberLength;
	integral_type Value = math::Abs(IntegralNumber);
	do {
		*--NumberStart = '0' + (Value % 10);
		Value /= 10;
	} while (Value);
	if (MinusSign) {
		*--NumberStart = '-';
	}
}

str FromPointer(void* Pointer) {
	const index Length = Pointer ? 18 : 4;
	str Result;
	Result.Reserve(Length + 1);
	WritePointer(Result.GetData(), Length, Pointer);
	Result[Length] = 0;
	Result.OverwriteSize(Length + 1);
	return Result;
}

void WritePointer(char* Destination, index Length, void* Pointer) {
	if (!Pointer) {
		std::memcpy(Destination, "null", 4);
		return;
	}
	constexpr str_view Table{"0123456789abcdef"};
	char* NumberStart = Destination + Length;
	u64 Value = std::bit_cast<u64>(Pointer);
	for (s32 Index = 0; Index < 16; ++Index) {
		u8 Digit = static_cast<u8>(Value & ((1 << 4) - 1));
		*--NumberStart = Table[Digit];
		Value >>= 4;
	}
	*--NumberStart = 'x';
	*--NumberStart = '0';
}

template <fractional float_type>
str FromFloat(float_type FloatingNumber) {
	const math::decimal_parts Parts = math::FloatToDecimal(FloatingNumber);
	const index NumberLength = math::GetNumDigits(Parts);
	str Result;
	Result.Reserve(NumberLength + 1);
	WriteFloat(Result.GetData(), NumberLength, Parts);
	Result[NumberLength] = 0;
	Result.OverwriteSize(NumberLength + 1);
	return Result;
}

void WriteFloat(char* Destination, index NumberLength, const math::decimal_parts& Parts) {
	if (Parts.IsNaN) {
		std::memcpy(Destination, "NaN", 3);
		return;
	}
	if (Parts.IsInfinity) {
		std::memcpy(Destination, Parts.IsNegative ? "-inf" : "inf", Parts.IsNegative ? 4 : 3);
		return;
	}
	if (Parts.Significand == 0) {
		std::memcpy(Destination, "0", 1);
		return;
	}
	char* NumberStart = Destination + NumberLength;
	const s32 NumSignificandDigits = math::GetNumDigits(Parts.Significand);
	s32 ExponentValue = math::Abs(Parts.Exponent + NumSignificandDigits - 1);
	do {
		*--NumberStart = '0' + (ExponentValue % 10);
		ExponentValue /= 10;
	} while (ExponentValue);
	if ((Parts.Exponent + NumSignificandDigits - 1) < 0) {
		*--NumberStart = '-';
	}
	*--NumberStart = 'e';
	s32 SignificandValue = math::Abs(Parts.Significand);
	do {
		*--NumberStart = '0' + (SignificandValue % 10);
		SignificandValue /= 10;
		if (SignificandValue && SignificandValue < 10) {
			*--NumberStart = '.';
		}
	} while (SignificandValue);
	if (Parts.IsNegative) {
		*--NumberStart = '-';
	}
}

}	 // namespace strings