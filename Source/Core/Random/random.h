#pragma once

#include "basic.h"
#include <random>

namespace random {

	// TODO: make actual state
	using random_state = std::mt19937_64; 

	FORCEINLINE u64 Uint64(random_state& RandomState) {
		std::uniform_int_distribution<u64> Dist;
	}
	
	FORCEINLINE float Float(random_state& RandomState) {
		double d;
		uint64_t x = Uint64(RandomState) >> 11;
		x = ((x + 1) >> 1) + (1022ull << 52);
		memcpy(&d, &x, sizeof(d));
		return d;
	}
}