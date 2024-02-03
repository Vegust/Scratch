#pragma once

#include "basic.h"
#include "Templates/result.h"
#include "str.h"

namespace strings {

template <typename type>
concept convertible_to_string = numeric<type> || pointer<type>;

template <integral integral_type>
struct default_int_format {
	static index GetCharSize(integral_type Value) {
		return math::GetNumDigits(Value);
	}

	static void Write(char* Destination, index Length, integral_type IntegralNumber) {
		const bool MinusSign = IntegralNumber < 0;
		char* NumberStart = Destination + Length;
		integral_type Value = math::Abs(IntegralNumber);
		do {
			*--NumberStart = '0' + (Value % 10);
			Value /= 10;
		} while (Value);
		if (MinusSign) {
			*--NumberStart = '-';
		}
	}
};

struct default_float_format {
	static index GetCharSize(const math::decimal_parts& Parts);
	static void Write(char* Destination, index Length, const math::decimal_parts& Parts);
};

struct default_pointer_format {
	static index GetCharSize(void* Value);
	static void Write(char* Destination, index Length, void* Value);
};

struct default_bool_format {
	static index GetCharSize(bool Value);
	static void Write(char* Destination, index Length, bool Value);
};

FORCEINLINE constexpr static index GetByteLength(str_view String);

FORCEINLINE constexpr bool IsSpace(char Character);
FORCEINLINE constexpr bool IsDigit(char Character);
FORCEINLINE constexpr bool IsSign(char Character);

template <convertible_to_string convertible_type>
static str ToString(const convertible_type& Value);

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
FORCEINLINE constexpr index FindFirstOf(str_view String, char Char);
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
	while (Length < Start.GetSize() && IsDigit(Start[Length])) {
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

constexpr bool IsDigit(char Character) {
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

constexpr index FindFirstOf(str_view String, char Char) {
	for (index Index = 0; Index < GetByteLength(String); ++Index) {
		if (String[Index] == Char) {
			return Index;
		}
	}
	return InvalidIndex;
}

constexpr index FindLastOf(str_view String, char Char) {
	for (s32 Index = GetByteLength(String) - 1; Index >= 0; --Index) {
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

template <convertible_to_string convertible_type>
str ToString(const convertible_type& Value) {
	str Result;
	index Length{0};
	if constexpr (std::is_same_v<convertible_type, bool>) {
		Length = default_bool_format::GetCharSize(Value);
		Result.Reserve(Length + 1);
		default_bool_format::Write(Result.GetData(), Length, Value);
	} else if constexpr (integral<convertible_type>) {
		Length = default_int_format<convertible_type>::GetCharSize(Value);
		Result.Reserve(Length + 1);
		default_int_format<convertible_type>::Write(Result.GetData(), Length, Value);
	} else if constexpr (fractional<convertible_type>) {
		auto Parts = math::FloatToDecimal(Value);
		Length = default_float_format::GetCharSize(Parts);
		Result.Reserve(Length + 1);
		default_float_format::Write(Result.GetData(), Length, Parts);
	} else if constexpr (pointer<convertible_type>) {
		Length = default_pointer_format::GetCharSize(Value);
		Result.Reserve(Length + 1);
		default_pointer_format::Write(Result.GetData(), Length, Value);
	}
	Result[Length] = 0;
	Result.OverwriteSize(Length + 1);
	return Result;
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

}	 // namespace strings