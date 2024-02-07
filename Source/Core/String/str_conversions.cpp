#include "str_conversions.h"
#include "fast_float.h"

namespace strings {
result<float> GetFloat(str_view String) {
	float Result;
	auto Answer = fast_float::from_chars(String.GetData(), String.GetData() + String.GetSize(), Result);
	if (static_cast<s32>(Answer.ec)) {
		return errors::CouldNotParseFloatFromString;	// TODO: actually look at error
	}
	return result<float>(Result);
}

result<double> GetDouble(str_view String) {
	double Result;
	auto Answer = fast_float::from_chars(String.GetData(), String.GetData() + String.GetSize(), Result);
	if (static_cast<s32>(Answer.ec)) {
		return errors::CouldNotParseFloatFromString;	// TODO: actually look at error
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

void default_float_format::Write(mutable_str_view Destination, const math::decimal_parts& Parts) {
	if (Parts.IsNaN) {
		std::memcpy(Destination.GetData(), "NaN", 3);
		return;
	}
	if (Parts.IsInfinity) {
		std::memcpy(Destination.GetData(), Parts.IsNegative ? "-inf" : "inf", Parts.IsNegative ? 4 : 3);
		return;
	}
	if (Parts.Significand == 0) {
		std::memcpy(Destination.GetData(), "0", 1);
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

index default_pointer_format::GetCharSize(void* Value) {
	if (!Value) {
		return 7;
	}
	return 18;
}

void default_pointer_format::Write(mutable_str_view Destination, void* Pointer) {
	if (!Pointer) {
		std::memcpy(Destination.GetData(), "nullptr", 7);
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

index default_bool_format::GetCharSize(bool) {
	return 4;
}

void default_bool_format::Write(mutable_str_view Destination, bool Value) {
	CHECK(Destination.GetSize() >= 4);
	std::memcpy(Destination.GetData(), Value ? "true" : "false", 4);
}

index default_timestamp_format::GetCharSize(timestamp) {
	// "DD-MM-YYYY hh:mm:ss.msm" = 23
	return 23;
}

void default_timestamp_format::Write(mutable_str_view Destination, timestamp Value) {
	using format = default_int_format<s64>;
	CHECK(Destination.GetSize() >= 23)
	std::memset(Destination.GetData(), '0', 23);
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

}	 // namespace strings