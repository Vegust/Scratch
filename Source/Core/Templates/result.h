#pragma once

enum class common_errors : u8 { out_of_memory, invalid_input, invalid_sign, input_too_big };

template <typename value_type, typename error_type = common_errors>
struct result {
private:
	union {
		value_type Value;
		error_type Error;
	};

	bool Success{false};

	static constexpr bool CopyableValue = sizeof(value_type) <= 16 && trivially_copyable<value_type>;
	static constexpr bool CopyableError = sizeof(error_type) <= 16 && trivially_copyable<error_type>;

public:
	FORCEINLINE constexpr result(const value_type InValue)
		requires CopyableValue
	{
		Value = InValue;
		Success = true;
	}

	FORCEINLINE constexpr result(const error_type InError)
		requires CopyableError
	{
		Error = InError;
		Success = false;
	}

	FORCEINLINE constexpr result(const value_type& InValue)
		requires(!CopyableValue)
	{
		Value = InValue;
		Success = true;
	}

	FORCEINLINE constexpr result(const error_type& InError)
		requires(!CopyableError)
	{
		Error = InError;
		Success = false;
	}

	FORCEINLINE constexpr result(value_type&& InValue)
		requires(!CopyableValue)
	{
		Value = std::move(InValue);
		Success = true;
	}

	FORCEINLINE constexpr result(error_type&& InError)
		requires(!CopyableValue)
	{
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