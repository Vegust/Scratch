#pragma once

#include "basic.h"
#include "Templates/result.h"
#include "str.h"
#include "Time/timestamp.h"
#include "fast_float.h"

namespace strings {
namespace errors {
inline const atom ThereIsNoNumbersToParse{"There is no numbers to parse."};
inline const atom ReadingSignedIntegralToUnsignedType{"Reading signed integral to unsigned type."};
inline const atom InputStringContainsNumberBiggerThatTypeCanHold{
	"Input string contains number bigger that type can hold"};
inline const atom CouldNotParseFloatFromString{"Could not parse float from string"};
}	 // namespace errors

FORCEINLINE constexpr void CopyChars(char* Destination, const char* Source, index Length) {
	if (!std::is_constant_evaluated()) {
		std::memcpy(Destination, Source, Length);
	} else {
		char* End = Destination + Length;
		while (Destination != End) {
			*Destination++ = *Source++;
		}
	}
}

FORCEINLINE constexpr void SetChars(char* Destination, char Value, index Length) {
	if (!std::is_constant_evaluated()) {
		std::memset(Destination, Value, Length);
	} else {
		char* End = Destination + Length;
		while (Destination != End) {
			*Destination++ = Value;
		}
	}
}

template <integral integral_type>
struct default_int_format {
	static constexpr index GetCharSize(integral_type Value) {
		return math::GetNumDigits(Value);
	}

	static constexpr void Write(mutable_str_view Destination, integral_type IntegralNumber) {
		const bool MinusSign = IntegralNumber < 0;
		char* NumberStart = Destination.GetData() + Destination.GetSize();
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
	static constexpr index GetCharSize(const math::decimal_parts& Parts) {
		const index NanLength = 3 * Parts.IsNaN;
		const index InfinityLength = (3 + Parts.IsNegative) * Parts.IsInfinity;
		const index ZeroLength = 1 * (Parts.Significand == 0) * !Parts.IsInfinity * !Parts.IsNaN;
		const index NumSignificandDigits = math::GetNumDigits(Parts.Significand);
		const s32 ExponentValue = Parts.Exponent + NumSignificandDigits - 1;
		const index NumExponentDigits = math::GetNumDigits(ExponentValue);
		const index SpecialLength = NanLength + InfinityLength + ZeroLength;
		const bool HasDot = NumSignificandDigits > 1;
		const index MaxLength = Parts.IsNegative + HasDot + 1 + NumExponentDigits + NumSignificandDigits;
		return MaxLength * (SpecialLength == 0) + SpecialLength;
	}

	static constexpr void Write(mutable_str_view Destination, const math::decimal_parts& Parts) {
		if (Parts.IsNaN) {
			CopyChars(Destination.GetData(), "NaN", 3);
			return;
		}
		if (Parts.IsInfinity) {
			CopyChars(Destination.GetData(), Parts.IsNegative ? "-inf" : "inf", Parts.IsNegative ? 4 : 3);
			return;
		}
		if (Parts.Significand == 0) {
			CopyChars(Destination.GetData(), "0", 1);
			return;
		}
		char* NumberStart = Destination.GetData() + Destination.GetSize();
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
};

struct default_pointer_format {
	static constexpr index GetCharSize(void* Value) {
		return (11 * (!!Value)) + 7;
	}

	static constexpr void Write(mutable_str_view Destination, void* Pointer) {
		if (!Pointer) {
			CopyChars(Destination.GetData(), "nullptr", 7);
			return;
		}
		constexpr str_view Table{"0123456789abcdef"};
		char* NumberStart = Destination.GetData() + Destination.GetSize();
		u64 Value = std::bit_cast<u64>(Pointer);
		for (s32 Index = 0; Index < 16; ++Index) {
			u8 Digit = static_cast<u8>(Value & ((1 << 4) - 1));
			*--NumberStart = Table[Digit];
			Value >>= 4;
		}
		*--NumberStart = 'x';
		*--NumberStart = '0';
	}
};

struct default_bool_format {
	static constexpr index GetCharSize(bool Value) {
		return 5 - Value;
	}

	static constexpr void Write(mutable_str_view Destination, bool Value) {
		CHECK(Destination.GetSize() >= (5 - Value));
		CopyChars(Destination.GetData(), Value ? "true" : "false", 5 - Value);
	}
};

struct default_timestamp_format {
	static constexpr index GetCharSize(timestamp Value) {
		// "DD-MM-YYYY hh:mm:ss.msm" = 23
		return 23;
	}

	static constexpr void Write(mutable_str_view Destination, timestamp Value) {
		using format = default_int_format<s64>;
		CHECK(Destination.GetSize() >= 23)
		SetChars(Destination.GetData(), '0', 23);
		timestamp::year_month_day YMD = Value.GetYearMonthDay();
		CHECK(format::GetCharSize(YMD.Year) <= 4);
		CHECK(format::GetCharSize(YMD.Month) <= 2);
		CHECK(format::GetCharSize(YMD.Day) <= 2);
		CHECK(format::GetCharSize(Value.GetHour()) <= 2);
		CHECK(format::GetCharSize(Value.GetMinute()) <= 2);
		CHECK(format::GetCharSize(Value.GetSecond()) <= 2);
		CHECK(format::GetCharSize(Value.GetMillisecond()) <= 3);
		format::Write(Destination.SliceFront(2), YMD.Day);
		Destination.SliceFront()[0] = '-';
		format::Write(Destination.SliceFront(2), YMD.Month);
		Destination.SliceFront()[0] = '-';
		format::Write(Destination.SliceFront(4), YMD.Year);
		Destination.SliceFront()[0] = ' ';
		format::Write(Destination.SliceFront(2), Value.GetHour());
		Destination.SliceFront()[0] = ':';
		format::Write(Destination.SliceFront(2), Value.GetMinute());
		Destination.SliceFront()[0] = ':';
		format::Write(Destination.SliceFront(2), Value.GetSecond());
		Destination.SliceFront()[0] = '.';
		format::Write(Destination.SliceFront(3), Value.GetMillisecond());
	}
};

FORCEINLINE constexpr static index GetByteLength(str_view String);

FORCEINLINE constexpr bool IsSpace(char Character);
FORCEINLINE constexpr bool IsDigit(char Character);
FORCEINLINE constexpr bool IsSign(char Character);

template <typename type>
concept convertible_to_string = numeric<type> || pointer<type>;
template <convertible_to_string convertible_type>
str ToString(const convertible_type& Value);

template <numeric numeric_type>
constexpr numeric_type GetNumberChecked(str_view String);
template <numeric numeric_type>
constexpr result<numeric_type> GetNumber(str_view String);
template <integral integral_type>
constexpr result<integral_type> GetInteger(str_view String);

constexpr result<float> GetFloat(str_view String) {
	float Result;
	auto Answer = fast_float::from_chars(String.GetData(), String.GetData() + String.GetSize(), Result);
	if (static_cast<s32>(Answer.ec)) {
		return errors::CouldNotParseFloatFromString;	// TODO: actually look at error
	}
	return result<float>(Result);
}

constexpr result<double> GetDouble(str_view String) {
	double Result;
	auto Answer = fast_float::from_chars(String.GetData(), String.GetData() + String.GetSize(), Result);
	if (static_cast<s32>(Answer.ec)) {
		return errors::CouldNotParseFloatFromString;	// TODO: actually look at error
	}
	return result<double>(Result);
}

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
		return errors::ThereIsNoNumbersToParse;
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
		Result.SliceFront();
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
		Tail.SliceFront();
	}
	return InvalidIndex;
}

template <convertible_to_string convertible_type>
str ToString(const convertible_type& Value) {
	str Result;
	index Length{0};
	if constexpr (std::is_same_v<convertible_type, bool>) {
		Length = default_bool_format::GetCharSize(Value);
		default_bool_format::Write(Result.AppendUninitialized(Length), Value);
	} else if constexpr (integral<convertible_type>) {
		Length = default_int_format<convertible_type>::GetCharSize(Value);
		default_int_format<convertible_type>::Write(Result.AppendUninitialized(Length), Value);
	} else if constexpr (fractional<convertible_type>) {
		auto Parts = math::FloatToDecimal(Value);
		Length = default_float_format::GetCharSize(Parts);
		default_float_format::Write(Result.AppendUninitialized(Length), Parts);
	} else if constexpr (pointer<convertible_type>) {
		Length = default_pointer_format::GetCharSize(Value);
		default_pointer_format::Write(Result.AppendUninitialized(Length), Value);
	}
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
constexpr result<integral_type> GetInteger(str_view String) {
	auto Result = GetIntegerString(String);
	if (Result) {
		str_view IntegerString = Result.GetValue();
		integral_type Sign = 1;
		if (IsSign(IntegerString[0])) {
			if constexpr (signed_integral<integral_type>) {
				Sign = IntegerString[0] == '-' ? -1 : 1;
			} else {
				if (IntegerString[0] == '-') {
					return errors::ReadingSignedIntegralToUnsignedType;
				}
			}
			IntegerString.SliceFront();
		}
		integral_type Number{0};
		while (!IntegerString.IsEmpty()) {
			const integral_type Digit = IntegerString[0] - '0';
			constexpr s32 MaxSafe = std::numeric_limits<integral_type>::max();
			if (Number > ((MaxSafe - Digit) / 10)) {
				return errors::InputStringContainsNumberBiggerThatTypeCanHold;
			}
			Number = Number * 10 + Digit;
			IntegerString.SliceFront();
		}
		return Number * Sign;
	}
	return Result.GetError();
}

}	 // namespace strings