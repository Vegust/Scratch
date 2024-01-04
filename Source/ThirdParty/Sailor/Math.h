#pragma once

#include "Defines.h"

namespace Sailor::Math
{
	template<typename T>
	SAILOR_API __forceinline T UpperPowOf2(T v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

	template<typename T>
	T Lerp(const T& a, const T& b, float t) { return a + (b - a) * t; }
}

#if defined(min)
#undef min
#define min(a,b) (a < b ? a : b)
#endif

#if defined(max)
#undef max
#define max(a,b) (a < b ? b : a)
#endif