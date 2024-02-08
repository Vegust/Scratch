#pragma once

#include "basic.h"
#include "vec.h"
#include "dragonbox.h"

#if defined(_MSC_VER)	 // MSVC, LLVM
#define BIT_SCAN_REVERSE_64(BIT_SCAN_REVERSE_Index, BIT_SCAN_REVERSE_Value) \
	_BitScanReverse64((BIT_SCAN_REVERSE_Index), (BIT_SCAN_REVERSE_Value))
#define BIT_SCAN_REVERSE_32(BIT_SCAN_REVERSE_Index, BIT_SCAN_REVERSE_Value) \
	_BitScanReverse((BIT_SCAN_REVERSE_Index), (BIT_SCAN_REVERSE_Value))
#endif

#if defined(__GNUC__)	 // GCC
static FORCEINLINE constexpr unsigned char BitScanReverseGNUC_64(unsigned long* Index, unsigned long long Mask) {
	const bool Zero = Mask == 0;
	if (Zero) {
		*Index = 0;
		return false;
	}
	*Index = __builtin_clzll(Mask);
	*Index = 63 - *Index;
	return true;
}

static FORCEINLINE constexpr unsigned char BitScanReverseGNUC_32(unsigned long* Index, unsigned long Mask) {
	const bool Zero = Mask == 0;
	if (Zero) {
		*Index = 0;
		return false;
	}
	*Index = __builtin_clzl(Mask);
	*Index = 31 - *Index;
	return true;
}

#define BIT_SCAN_REVERSE_64(BIT_SCAN_REVERSE_Index, BIT_SCAN_REVERSE_Value) \
	BitScanReverseGNUC_64((BIT_SCAN_REVERSE_Index), (BIT_SCAN_REVERSE_Value))
#define BIT_SCAN_REVERSE_32(BIT_SCAN_REVERSE_Index, BIT_SCAN_REVERSE_Value) \
	BitScanReverseGNUC_32((BIT_SCAN_REVERSE_Index), (BIT_SCAN_REVERSE_Value))
#endif

template <integral integral_type>
constexpr char BitScanReverse(unsigned long* Index, integral_type Mask) {
	if (std::is_constant_evaluated()) {
		unsigned long Count = 0;
		while (Mask != 0) {
			const bool HasBit = (Mask & 1);
			*Index = HasBit * Count + !HasBit * *Index;
			Mask = Mask >> 1;
			++Count;
		}
		return Count > 0;
	}
	if constexpr (sizeof(integral_type) > 4) {
		return BIT_SCAN_REVERSE_64(Index, Mask);
	} else {
		return BIT_SCAN_REVERSE_32(Index, Mask);
	}
}

namespace math {
constexpr float DefaultEqualityTolerance = 0.001f;

struct decimal_parts {
	u64 Significand{0};
	s16 Exponent{0};
	bool IsNegative{false};
	bool IsNaN{false};
	bool IsInfinity{false};
};

template <fractional float_type>
constexpr decimal_parts CalcFloatParts(float_type Number) {
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
		return Result;	  // 0
	} else {
		Result.IsNegative = Number < 0;
		// TODO: expose policies as configuration
		auto ValidParts = jkj::dragonbox::to_decimal(Number);
		Result.Exponent = ValidParts.exponent;
		Result.Significand = ValidParts.significand;
	}
	return Result;
}

FORCEINLINE constexpr decimal_parts FloatToDecimal(float Number) {
	return CalcFloatParts(Number);
}

FORCEINLINE constexpr decimal_parts FloatToDecimal(double Number) {
	return CalcFloatParts(Number);
}

template <typename lhs, typename rhs>
	requires(sizeof(lhs) <= 16 && sizeof(rhs) <= 16)
FORCEINLINE constexpr std::common_type<lhs, rhs>::type Max(lhs a, rhs b) {
	return ((a) > (b) ? (a) : (b));
}

template <typename lhs, typename rhs>
	requires(sizeof(lhs) <= 16 && sizeof(rhs) <= 16)
FORCEINLINE constexpr std::common_type<lhs, rhs>::type Min(lhs a, rhs b) {
	return ((a) < (b) ? (a) : (b));
}

template <numeric numeric_type>
FORCEINLINE constexpr numeric_type Abs(numeric_type Value) {
	return (Value > 0) ? Value : -Value;
}

// includes sign
template <integral integral_type>
FORCEINLINE constexpr index GetNumDigits(integral_type Value) {
	const bool MinusSign = Value < 0;
	Value = math::Abs(Value);
	constexpr u8 BsrToLog10[] = {1,	 1,	 1,	 2,	 2,	 2,	 3,	 3,	 3,	 4,	 4,	 4,	 4,	 5,	 5,	 5,	 6,	 6,	 6,	 7,	 7,	 7,
								 7,	 8,	 8,	 8,	 9,	 9,	 9,	 10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14,
								 14, 14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20};
	unsigned long Bsr = 0;
	BitScanReverse(&(unsigned long&) Bsr, Value);
	u8 Log10 = BsrToLog10[Bsr];
	constexpr u64 Pow10[] = {
		0,
		0,
		10U,
		100U,
		1000U,
		10000U,
		100000U,
		1000000U,
		10000000U,
		100000000U,
		1000000000U,
		10000000000ULL,
		100000000000ULL,
		1000000000000ULL,
		10000000000000ULL,
		100000000000000ULL,
		1000000000000000ULL,
		10000000000000000ULL,
		100000000000000000ULL,
		1000000000000000000ULL,
		10000000000000000000ULL};
	return MinusSign + Log10 - (Value < Pow10[Log10]);
}

template <integral integral_type>
constexpr FORCEINLINE integral_type LogOfTwoCeil(integral_type Value) {
	unsigned long Index;
	if constexpr (sizeof(integral_type) > 4) {
		if (BIT_SCAN_REVERSE_64(&Index, (u64) Value)) {
			integral_type result = Index;
			if ((Value & ~((integral_type) 1 << Index)) > 0) {
				++result;
			}
			return result;
		} else {
			return (integral_type) 0;
		}
	} else {
		if (BIT_SCAN_REVERSE_32(&Index, (u32) Value)) {
			integral_type result = Index;
			if ((Value & ~((integral_type) 1 << Index)) > 0) {
				++result;
			}
			return result;
		} else {
			return (integral_type) 0;
		}
	}
}

// Component-wise
template <numeric number_type>
FORCEINLINE vector2<number_type> Max(vector2<number_type> Lhs, vector2<number_type> Rhs);
// Component-wise
template <numeric number_type>
FORCEINLINE vector2<number_type> Min(vector2<number_type> Lhs, vector2<number_type> Rhs);

template <fractional number_type>
FORCEINLINE bool Equal(
	vector2<number_type> Lhs,
	vector2<number_type> Rhs,
	number_type Tolerance = DefaultEqualityTolerance);

template <fractional number_type>
FORCEINLINE number_type Dot(vector2<number_type> Lhs, vector2<number_type> Rhs);
template <fractional number_type>
FORCEINLINE number_type Cross(vector2<number_type> Lhs, vector2<number_type> Rhs);
template <fractional number_type>
FORCEINLINE vector2<number_type> Normalise(vector2<number_type> Vector);
template <fractional number_type>
FORCEINLINE number_type Distance(vector2<number_type> Lhs, vector2<number_type> Rhs);
template <fractional number_type>
FORCEINLINE number_type DistanceSquared(vector2<number_type> Lhs, vector2<number_type> Rhs);

}	 // namespace math