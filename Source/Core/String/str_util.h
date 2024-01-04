#pragma once

#include "basic.h"
#include "str.h"

namespace str_util {

template <integral integral_type>

[[nodiscard]] static str FromInt(integral_type IntegralNumber) {
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

template <fractional float_type>
[[nodiscard]] static str FromFloat(float_type FloatingNumber, index FractionalPrecision = 4) {
	return {}; // TODO
}

}	 // namespace str_util