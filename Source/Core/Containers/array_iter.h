#pragma once

#include "core_types.h"

// shared between array/dyn_array/span
template <typename array_type, bool Const>
class array_iter {
public:
	using value_type = array_type::value_type;
	using pointer = std::conditional<Const, const value_type*, value_type*>::type;
	using reference = std::conditional<Const, const value_type&, value_type&>::type;

	constexpr static bool Contiguous = true;

private:
	pointer mElement = nullptr;

public:
	FORCEINLINE constexpr array_iter() = default;
	FORCEINLINE constexpr array_iter(const array_iter&) = default;
	FORCEINLINE constexpr array_iter(array_iter&&) noexcept = default;
	FORCEINLINE constexpr ~array_iter() = default;

	FORCEINLINE constexpr explicit array_iter(pointer Element) : mElement(Element) {
	}

	// implicit conversion
	FORCEINLINE constexpr operator pointer() const {
		return mElement;
	}

	FORCEINLINE constexpr array_iter& operator++() {
		mElement++;
		return *this;
	}

	FORCEINLINE constexpr const array_iter operator++(int) {
		auto Tmp = *this;
		mElement++;
		return Tmp;
	}

	FORCEINLINE constexpr array_iter& operator--() {
		mElement--;
		return *this;
	}

	FORCEINLINE constexpr const array_iter operator--(int) {
		auto Tmp = *this;
		mElement--;
		return Tmp;
	}

	FORCEINLINE constexpr array_iter operator-(int Val) const {
		return array_iter(mElement - Val);
	}

	FORCEINLINE constexpr array_iter operator+(int Val) const {
		return array_iter(mElement + Val);
	}

	FORCEINLINE constexpr reference operator[](index Index) {
		return *(mElement + Index);
	}

	FORCEINLINE constexpr pointer operator->() {
		return mElement;
	}

	FORCEINLINE constexpr reference operator*() {
		return *mElement;
	}

	FORCEINLINE constexpr bool operator==(const array_iter& Other) const {
		return mElement == Other.mElement;
	}
};