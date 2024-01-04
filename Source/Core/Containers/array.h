#pragma once

#include "basic.h"
#include "array_iter.h"
#include <cstring>
#include <initializer_list>

template <typename element_type, index SizeParameter>
struct array {
	constexpr static index Size = SizeParameter;
	element_type Data[Size]{};

	using value_type = element_type;
	using iter = array_iter<array, iterator_constness::non_constant>;
	using const_iter = array_iter<array, iterator_constness::constant>;

	constexpr array() = default;
	constexpr ~array() = default;

	FORCEINLINE constexpr explicit array(const element_type* InSource, index InSize) {
		CopyConstructElements(InSource, InSize);
	}

	FORCEINLINE constexpr array(std::initializer_list<element_type> InitList)
		: array(InitList.begin(), InitList.size()) {
	}

	FORCEINLINE constexpr array(const array& Other) {
		for (s32 i = 0; i < Size; ++i) {
			Data[i] = Other.Data[i];
		}
	}

	FORCEINLINE constexpr array(const array&& Other) noexcept {
		for (s32 i = 0; i < Size; ++i) {
			Data[i] = std::move(Other.Data[i]);
		}
	}

	template <index OtherSize>
	FORCEINLINE constexpr explicit array(const array<element_type, OtherSize>& Other) {
		CopyConstructElements(Other.Data, Other.Size);
	}

	template <index OtherSize>
	FORCEINLINE constexpr explicit array(array<element_type, OtherSize>&& Other) noexcept {
		MoveConstructElements(Other.Data, Other.Size);
	}

	FORCEINLINE constexpr bool operator==(const array& Other) const {
		for (index i = 0; i < Size; ++i) {
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
		MoveConstructElements(Other.Data, Other.Size);
		return *this;
	}

	FORCEINLINE constexpr array& operator=(const array& Other) {
		if (&Other == this) {
			return *this;
		}
		CopyConstructElements(Other.Data, Other.Size);
		return *this;
	}

	FORCEINLINE constexpr element_type* GetData() {
		return Data;
	}

	FORCEINLINE constexpr const element_type* GetData() const {
		return Data;
	}

	[[nodiscard]] FORCEINLINE constexpr index GetSize() const {
		return Size;
	}

	FORCEINLINE constexpr element_type& operator[](const index Index) {
		return Data[Index];
	}

	FORCEINLINE constexpr const element_type& operator[](const index Index) const {
		return Data[Index];
	}

	FORCEINLINE constexpr void CopyConstructElements(const element_type* Source, index Size) {
		for (index i = 0; i < Size; ++i) {
			Data[i] = element_type(Source[i]);
		}
	}

	FORCEINLINE constexpr void MoveConstructElements(element_type* Source, index Size) {
		for (index i = 0; i < Size; ++i) {
			Data[i] = element_type(std::move(Source[i]));
		}
	}

	FORCEINLINE constexpr iter begin() {
		return iter(Data);
	}

	FORCEINLINE constexpr iter end() {
		return iter(Data + Size);
	}

	FORCEINLINE constexpr const_iter begin() const {
		return const_iter(Data);
	}

	FORCEINLINE constexpr const_iter end() const {
		return const_iter(Data + Size);
	}
};
