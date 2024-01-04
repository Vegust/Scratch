#pragma once

#include "basic.h"

// shared between array/dyn_array/span
template <typename array_type, iterator_constness Constness>
class array_iter {
public:
	using value_type = array_type::value_type;
	using pointer = std::conditional<Constness == iterator_constness::constant, const value_type*, value_type*>::type;
	using reference = std::conditional<Constness == iterator_constness::constant, const value_type&, value_type&>::type;

	constexpr static bool Contiguous = true;

private:
	pointer Element = nullptr;

public:
	FORCEINLINE constexpr array_iter() = default;
	FORCEINLINE constexpr array_iter(const array_iter&) = default;
	FORCEINLINE constexpr array_iter(array_iter&&) noexcept = default;
	FORCEINLINE constexpr ~array_iter() = default;

	FORCEINLINE constexpr explicit array_iter(pointer InElement) : Element(InElement) {
	}

	// implicit conversion
	FORCEINLINE constexpr operator pointer() const {	// NOLINT(*-explicit-constructor)
		return Element;
	}

	FORCEINLINE constexpr array_iter& operator++() {
		Element++;
		return *this;
	}

	FORCEINLINE constexpr const array_iter operator++(int) {
		auto Tmp = *this;
		Element++;
		return Tmp;
	}

	FORCEINLINE constexpr array_iter& operator--() {
		Element--;
		return *this;
	}

	FORCEINLINE constexpr const array_iter operator--(int) {
		auto Tmp = *this;
		Element--;
		return Tmp;
	}

	FORCEINLINE constexpr array_iter operator-(int Val) const {
		return array_iter(Element - Val);
	}

	FORCEINLINE constexpr array_iter operator+(int Val) const {
		return array_iter(Element + Val);
	}

	FORCEINLINE constexpr reference operator[](index Index) {
		return *(Element + Index);
	}

	FORCEINLINE constexpr pointer operator->() {
		return Element;
	}

	FORCEINLINE constexpr reference operator*() {
		return *Element;
	}

	FORCEINLINE constexpr bool operator==(const array_iter& Other) const {
		return Element == Other.Element;
	}
};