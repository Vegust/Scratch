#pragma once

#include "core_types.h"

//shared between array/dyn_array/span
template <typename array_type, bool Const>
class array_iter {
public:
	using value_type = array_type::value_type;
	using pointer = std::conditional<Const, const value_type*, value_type*>::type;
	using reference = std::conditional<Const, const value_type&, value_type&>::type;
	
private:
	pointer mElement = nullptr;

public:
	FORCEINLINE array_iter() = default;
	FORCEINLINE array_iter(const array_iter&) = default;
	FORCEINLINE array_iter(array_iter&&) noexcept = default;
	FORCEINLINE ~array_iter() = default;

	FORCEINLINE explicit array_iter(pointer Element) : mElement(Element) {
	}

	// implicit conversion
	FORCEINLINE operator pointer() const {
		return mElement;
	}

	FORCEINLINE array_iter& operator++() {
		mElement++;
		return *this;
	}

	FORCEINLINE const array_iter operator++(int) {
		auto Tmp = *this;
		mElement++;
		return Tmp;
	}

	FORCEINLINE array_iter& operator--() {
		mElement--;
		return *this;
	}

	FORCEINLINE const array_iter operator--(int) {
		auto Tmp = *this;
		mElement--;
		return Tmp;
	}

	FORCEINLINE array_iter operator-(int Val) const {
		return array_iter(mElement - Val);
	}

	FORCEINLINE array_iter operator+(int Val) const {
		return array_iter(mElement + Val);
	}

	FORCEINLINE reference operator[](index_type Index) {
		return *(mElement + Index);
	}

	FORCEINLINE pointer operator->() {
		return mElement;
	}

	FORCEINLINE reference operator*() {
		return *mElement;
	}

	FORCEINLINE bool operator==(const array_iter& Other) const {
		return mElement == Other.mElement;
	}
};