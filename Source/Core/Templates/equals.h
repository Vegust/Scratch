#pragma once

struct default_equals_op {
	template <typename lhs_type, typename rhs_type>
	FORCEINLINE constexpr bool operator()(lhs_type&& Lhs, rhs_type&& Rhs) const {
		return Lhs == Rhs;
	}

	template <typename lhs_type, typename rhs_type>
	FORCEINLINE constexpr static bool Equals(lhs_type&& Lhs, rhs_type&& Rhs) {
		return Lhs == Rhs;
	}
};