#include "math.h"
#include "dragonbox.h"
#include <bit>

namespace math {

template <fractional float_type>
static constexpr decimal_parts CalcFloatParts(float_type Number) {
	using carrier_type = std::conditional<sizeof(float_type) == 4, u32, u64>::type;

	constexpr carrier_type SignificandSize = sizeof(float_type) == 4 ? 23 : 52;
	constexpr carrier_type ExponentSize = sizeof(float_type) == 4 ? 8 : 11;
	constexpr carrier_type SignificandMask = (static_cast<u64>(1) << SignificandSize) - 1;
	constexpr carrier_type ExponentMask = (static_cast<u64>(1) << ExponentSize) - 1;

	const carrier_type NumberBits = std::bit_cast<carrier_type>(Number);
	const carrier_type BinarySignificand = NumberBits & SignificandMask;
	const carrier_type BinaryExponent = (NumberBits >> SignificandSize) & ExponentMask;

	decimal_parts Result;
	if (BinaryExponent == ExponentMask) {
		if (BinarySignificand == 0) {
			Result.IsNegative = Number < 0;
			Result.IsInfinity = true;
		} else {
			Result.IsNaN = true;
		}
	} else if (BinaryExponent == 0 && BinarySignificand == 0) {
		return Result; // 0
	} else {
		Result.IsNegative = Number < 0;
		// TODO: expose policies as configuration
		auto ValidParts = jkj::dragonbox::to_decimal(Number);
		Result.Exponent = ValidParts.exponent;
		Result.Significand = ValidParts.significand;
	}

	return Result;
}

decimal_parts FloatToDecimal(float Number) {
	return CalcFloatParts(Number);
}

decimal_parts FloatToDecimal(double Number) {
	return CalcFloatParts(Number);
}

}	 // namespace math