#pragma once

#include "String/atom.h"

template <typename value_type, typename error_type = atom>
struct result {
private:
	union {
		value_type Value;
		error_type Error;
	};

	bool Success{false};

public:
	FORCEINLINE constexpr result(const value_type& InValue) { // NOLINT(*-explicit-constructor)
		Value = InValue;
		Success = true;
	}

	FORCEINLINE constexpr result(const error_type& InError) { // NOLINT(*-explicit-constructor)
		Error = InError;
		Success = false;
	}

	FORCEINLINE constexpr result(value_type&& InValue) { // NOLINT(*-explicit-constructor)
		Value = std::move(InValue);
		Success = true;
	}

	FORCEINLINE constexpr result(error_type&& InError) { // NOLINT(*-explicit-constructor)
		Error = std::move(InError);
		Success = false;
	}

	FORCEINLINE constexpr operator bool() const {
		return Success;
	}

	FORCEINLINE constexpr bool IsSuccess() const {
		return Success;
	}

	FORCEINLINE constexpr const value_type& GetValue() const {
		return Value;
	}

	FORCEINLINE constexpr const error_type& GetError() const {
		return Error;
	}
};