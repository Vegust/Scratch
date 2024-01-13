#include "str_conversions.h"
#include "fast_float.h"

namespace strings {
result<float> GetFloat(str_view String) {
	float Result;
	auto Answer = fast_float::from_chars(String.GetData(),String.GetData() + String.GetSize(), Result);
	if (static_cast<s32>(Answer.ec)) {
		return common_errors::who_knows; // TODO: actually look at error
	}
	return result<float>(Result);
}

result<double> GetDouble(str_view String) {
	double Result;
	auto Answer = fast_float::from_chars(String.GetData(),String.GetData() + String.GetSize(), Result);
	if (static_cast<s32>(Answer.ec)) {
		return common_errors::who_knows; // TODO: actually look at error
	}
	return result<double>(Result);
}

index default_float_format::GetCharSize(const math::decimal_parts& Parts) {
	if (Parts.IsNaN) {
		return 3;
	}
	if (Parts.IsInfinity) {
		return Parts.IsNegative ? 4 : 3;
	}
	if (Parts.Significand == 0) {
		return 1;
	}
	const s32 NumSignificandDigits = math::GetNumDigits(Parts.Significand);
	s32 ExponentValue = Parts.Exponent + NumSignificandDigits - 1;
	const bool HasDot = NumSignificandDigits > 1;
	return Parts.IsNegative + HasDot + 1 /* 'e' */ + math::GetNumDigits(ExponentValue) + NumSignificandDigits;
}

void default_float_format::Write(char* Destination, index Length, const math::decimal_parts& Parts) {
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
	char* NumberStart = Destination + Length;
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

index default_pointer_format::GetCharSize(void* Value) {
	if (!Value) {
		return 4;
	}
	return 18;
}

void default_pointer_format::Write(char* Destination, index Length, void* Pointer) {
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

index default_bool_format::GetCharSize(bool Value) {
	return 4;
}

void default_bool_format::Write(char* Destination, index Length, bool Value) {
	std::memcpy(Destination, Value ? "true" : "false", 4);
}
}