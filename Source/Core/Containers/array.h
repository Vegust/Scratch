#pragma once

#include "core_types.h"
#include <cstring>

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

	FORCEINLINE array_iter operator++(int) {
		auto Tmp = *this;
		mElement++;
		return Tmp;
	}

	FORCEINLINE array_iter& operator--() {
		mElement--;
		return *this;
	}

	FORCEINLINE array_iter operator--(int) {
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

template <typename element_type, index_type SizeParameter>
struct array {
	constexpr static index_type mSize = SizeParameter;
	element_type mData[mSize]{};

	using value_type = element_type;
	using iter = array_iter<array, false>;
	using const_iter = array_iter<array, true>;

	constexpr array() = default;
	constexpr ~array() = default;

	FORCEINLINE constexpr explicit array(const element_type* Source, index_type Size) {
		CopyConstructElements(Source, Size);
	}

	FORCEINLINE constexpr array(std::initializer_list<element_type> InitList)
		: array(InitList.begin(), InitList.size()) {
	}

	FORCEINLINE constexpr array(const array& Other) : mData{Other.mData} {
	}

	FORCEINLINE constexpr array(const array&& Other) noexcept : mData{std::move(Other.mData)} {
	}

	template <index_type OtherSize>
	FORCEINLINE constexpr explicit array(const array<element_type, OtherSize>& Other) {
		CopyConstructElements(Other.mData, Other.mSize);
	}

	template <index_type OtherSize>
	FORCEINLINE constexpr explicit array(const array<element_type, OtherSize>&& Other) noexcept {
		MoveConstructElements(Other.mData, Other.mSize);
	}

	FORCEINLINE constexpr bool operator==(const array& Other) const {
		for (index_type i = 0; i < mSize; ++i) {
			if (Other[i] != operator[](i)) {
				return false;
			}
		}
		return true;
	}

	FORCEINLINE constexpr array& operator=(std::initializer_list<element_type> InitList) {
		CopyConstructElements(InitList.begin(), InitList.size());
		return *this;
	}

	FORCEINLINE constexpr array& operator=(array&& Other) noexcept {
		if (&Other == this) {
			return *this;
		}
		MoveConstructElements(Other.mData, Other.mSize);
		return *this;
	}

	FORCEINLINE constexpr array& operator=(const array& Other) {
		if (&Other == this) {
			return *this;
		}
		CopyConstructElements(Other.mData, Other.mSize);
		return *this;
	}

	FORCEINLINE constexpr element_type* Data() {
		return mData;
	}

	FORCEINLINE constexpr const element_type* Data() const {
		return mData;
	}

	[[nodiscard]] FORCEINLINE constexpr index_type Size() const {
		return mSize;
	}

	FORCEINLINE constexpr element_type& operator[](const index_type Index) {
		return mData[Index];
	}

	FORCEINLINE constexpr const element_type& operator[](const index_type Index) const {
		return mData[Index];
	}

	FORCEINLINE constexpr void CopyConstructElements(const element_type* Source, index_type Size) {
		for (index_type i = 0; i < Size; ++i) {
			mData[i] = element_type(Source[i]);
		}
	}

	FORCEINLINE constexpr void MoveConstructElements(const element_type* Source, index_type Size) {
		for (index_type i = 0; i < Size; ++i) {
			mData[i] = element_type(std::move(Source[i]));
		}
	}

	FORCEINLINE constexpr iter begin() {
		return iter(mData);
	}

	FORCEINLINE constexpr iter end() {
		return iter(mData + mSize);
	}

	FORCEINLINE constexpr const_iter begin() const {
		return const_iter(mData);
	}

	FORCEINLINE constexpr const_iter end() const {
		return const_iter(mData + mSize);
	}
};
