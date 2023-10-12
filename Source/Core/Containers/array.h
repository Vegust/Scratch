﻿#pragma once

#include "core_types.h"
#include "array_iter.h"
#include <cstring>

template <typename element_type, index SizeParameter>
struct array {
	constexpr static index mSize = SizeParameter;
	element_type mData[mSize]{};

	using value_type = element_type;
	using iter = array_iter<array, false>;
	using const_iter = array_iter<array, true>;

	constexpr array() = default;
	constexpr ~array() = default;

	FORCEINLINE constexpr explicit array(const element_type* Source, index Size) {
		CopyConstructElements(Source, Size);
	}

	FORCEINLINE constexpr array(std::initializer_list<element_type> InitList)
		: array(InitList.begin(), InitList.size()) {
	}

	FORCEINLINE constexpr array(const array& Other) {
		for (s32 i = 0; i < mSize; ++i) {
			mData[i] = Other.mData[i];
		}
	}

	FORCEINLINE constexpr array(const array&& Other) noexcept {
		for (s32 i = 0; i < mSize; ++i) {
			mData[i] = std::move(Other.mData[i]);
		}
	}

	template <index OtherSize>
	FORCEINLINE constexpr explicit array(const array<element_type, OtherSize>& Other) {
		CopyConstructElements(Other.mData, Other.mSize);
	}

	template <index OtherSize>
	FORCEINLINE constexpr explicit array(const array<element_type, OtherSize>&& Other) noexcept {
		MoveConstructElements(Other.mData, Other.mSize);
	}

	FORCEINLINE constexpr bool operator==(const array& Other) const {
		for (index i = 0; i < mSize; ++i) {
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

	[[nodiscard]] FORCEINLINE constexpr index Size() const {
		return mSize;
	}

	FORCEINLINE constexpr element_type& operator[](const index Index) {
		return mData[Index];
	}

	FORCEINLINE constexpr const element_type& operator[](const index Index) const {
		return mData[Index];
	}

	FORCEINLINE constexpr void CopyConstructElements(const element_type* Source, index Size) {
		for (index i = 0; i < Size; ++i) {
			mData[i] = element_type(Source[i]);
		}
	}

	FORCEINLINE constexpr void MoveConstructElements(const element_type* Source, index Size) {
		for (index i = 0; i < Size; ++i) {
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
