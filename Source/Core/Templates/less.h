#pragma once

#include "basic.h"

struct default_less_op {
	template <typename lhs_type, typename rhs_type>
	FORCEINLINE constexpr bool operator()(lhs_type&& Lhs, rhs_type&& Rhs) const {
		return Lhs < Rhs;
	}

	template <typename lhs_type, typename rhs_type>
	FORCEINLINE constexpr static bool Less(lhs_type&& Lhs, rhs_type&& Rhs) {
		return Lhs < Rhs;
	}
};