#pragma once

#include "basic.h"
#include "vec.h"

#if defined(_MSC_VER)	 // MSVC, LLVM
#define BIT_SCAN_REVERSE_64(BIT_SCAN_REVERSE_Index, BIT_SCAN_REVERSE_Value) \
	_BitScanReverse64((BIT_SCAN_REVERSE_Index), (BIT_SCAN_REVERSE_Value))
#define BIT_SCAN_REVERSE_32(BIT_SCAN_REVERSE_Index, BIT_SCAN_REVERSE_Value) \
	_BitScanReverse((BIT_SCAN_REVERSE_Index), (BIT_SCAN_REVERSE_Value))
#endif

#if defined(__GNUC__)	 // GCC
static FORCEINLINE unsigned char BitScanReverseGNUC_64(unsigned long* Index, unsigned long long Mask) {
	const bool Zero = Mask == 0;
	if (Zero) {
		*Index = 0;
		return false;
	}
	*Index = __builtin_clzll(Mask);
	*Index = 63 - *Index;
	return true;
}

static FORCEINLINE unsigned char BitScanReverseGNUC_32(unsigned long* Index, unsigned long Mask) {
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

namespace math {
constexpr float DefaultEqualityTolerance = 0.001f;

struct decimal_parts {
	s64 Exponent{0};
	u64 Significand{0};
	bool IsNegative{false};
	bool IsNaN{false};
	bool IsInfinity{false};
};

decimal_parts FloatToDecimal(float Number);
decimal_parts FloatToDecimal(double Number);

template <typename lhs, typename rhs>
	requires(sizeof(lhs) <= 16 && sizeof(rhs) <= 16)
constexpr FORCEINLINE std::common_type<lhs, rhs>::type Max(lhs a, rhs b) {
	return ((a) > (b) ? (a) : (b));
}

template <typename lhs, typename rhs>
	requires(sizeof(lhs) <= 16 && sizeof(rhs) <= 16)
constexpr FORCEINLINE std::common_type<lhs, rhs>::type Min(lhs a, rhs b) {
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
	constexpr static u8 BsrToLog10[] = {1,	1,	1,	2,	2,	2,	3,	3,	3,	4,	4,	4,	4,	5,	5,	5,
										6,	6,	6,	7,	7,	7,	7,	8,	8,	8,	9,	9,	9,	10, 10, 10,
										10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15,
										15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20};
	volatile unsigned long Bsr = 0;
	BIT_SCAN_REVERSE_64(&(unsigned long&)Bsr, Value);
	u8 Log10 = BsrToLog10[Bsr];
	constexpr static u64 Pow10[] = {
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