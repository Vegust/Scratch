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
	*Index = __builtin_clzll(Mask);
	bool Zero = *Index == 64;
	if (Zero) {
		*Index = 0;
		return false;
	}
	*Index = 63 - *Index;
	return true;
}
static FORCEINLINE unsigned char BitScanReverseGNUC_32(unsigned long* Index, unsigned long Mask) {
	*Index = __builtin_clzl(Mask);
	bool Zero = *Index == 32;
	if (Zero) {
		*Index = 0;
		return false;
	}
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