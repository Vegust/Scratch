#pragma once

#include "core_types.h"

#include <intrin.h>

#include <cstring>

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

template <typename swapped_type>
constexpr FORCEINLINE void Memswap(swapped_type& a, swapped_type& b) {
	alignas(swapped_type) u8 tmp_storage[sizeof(swapped_type)] = {0};
	std::memcpy(&tmp_storage, &a, sizeof(swapped_type));
	std::memcpy(&a, &b, sizeof(swapped_type));
	std::memcpy(&b, &tmp_storage, sizeof(swapped_type));
}

template <integral integral_type>
constexpr FORCEINLINE integral_type LogOfTwoCeil(integral_type Value) {
	unsigned long Index;
	if constexpr (sizeof(integral_type) > 4) {
		if (_BitScanReverse64(&Index, (u64) Value)) {
			integral_type result = Index;
			if ((Value & ~((integral_type) 1 << Index)) > 0) {
				++result;
			}
			return result;
		} else {
			return (integral_type) 0;
		}
	} else {
		if (_BitScanReverse(&Index, (u32) Value)) {
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
