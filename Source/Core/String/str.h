﻿#pragma once

#include "basic.h"
#include "Containers/array.h"
#include "Containers/dyn_array.h"
#include "Containers/span.h"
#include "Hash/hash.h"

#include <iostream>

using mutable_str_view = mutable_span<char>;
using str_view = span<char>;

struct str {
public:
	using char_type = char;
	using value_type = char;

	constexpr static index StackSize = 16;
	constexpr static bool MemcopyRelocatable = true;

	using array_type = dyn_array<char_type, default_allocator, StackSize>;
	using iter = array_type::iter;
	using const_iter = array_type::const_iter;

private:
	array_type Bytes{};

public:
	// defines constexpr analogs to strlen etc
	using traits = std::char_traits<char_type>;

	str() = default;

	explicit str(const str_view Other) : str(Other.GetData(), Other.GetSize()) {
	}

	str(const char* Characters, index Length) {
		std::memcpy(AppendUninitialized(Length).GetData(), Characters, Length);
	}

	str(const str& Other) : Bytes(Other.Bytes) {
	}

	str(str&& Other) : Bytes(std::move(Other.Bytes)) {
	}

	str(char Char, const str& String) {
		const index RequiredSize = String.GetByteLength() + 1;
		std::memcpy(AppendUninitialized(RequiredSize).GetData() + 1, String.GetData(), RequiredSize);
		Bytes[0] = Char;
	}

	FORCEINLINE str& operator=(str&& Other) noexcept {
		Bytes = std::move(Other.Bytes);
		return *this;
	}

	FORCEINLINE str& operator=(const str& Other) {
		Bytes = Other.Bytes;
		return *this;
	}

	FORCEINLINE str& operator=(const str_view Other) {
		Bytes.Clear(container_clear_type::dont_deallocate);
		std::memcpy(AppendUninitialized(Other.GetSize()).GetData(), Other.GetData(), Other.GetSize());
		return *this;
	}

	FORCEINLINE char_type* GetData() {
		return Bytes.GetData();
	}

	[[nodiscard]] FORCEINLINE const char_type* GetData() const {
		return Bytes.GetData();
	}

	FORCEINLINE char* GetRaw() {
		return (char*) Bytes.GetData();
	}

	[[nodiscard]] FORCEINLINE const char* GetRaw() const {
		return (char*) Bytes.GetData();
	}

	// bytes explicitly stated to distinguish from character length
	// does not include null terminator
	[[nodiscard]] FORCEINLINE index GetByteLength() const {
		return Bytes.GetSize() ? Bytes.GetSize() - 1 : 0;
	}

	[[nodiscard]] FORCEINLINE index IsEmpty() const {
		return GetByteLength() == 0;
	}

	FORCEINLINE char_type& operator[](const index Index) {
		return Bytes[Index];
	}

	FORCEINLINE const char_type& operator[](const index Index) const {
		return Bytes[Index];
	}

	FORCEINLINE char_type& GetAt(index Position) {
		return Bytes[Position];
	}

	[[nodiscard]] FORCEINLINE const char_type& GetAt(index Position) const {
		return Bytes[Position];
	}

	FORCEINLINE iter begin() {
		return Bytes.begin();
	}

	FORCEINLINE iter end() {
		return Bytes.end() - (GetByteLength() > 0 ? 1 : 0);
	}

	[[nodiscard]] FORCEINLINE const_iter begin() const {
		return Bytes.begin();
	}

	[[nodiscard]] FORCEINLINE const_iter end() const {
		return Bytes.end() - (GetByteLength() > 0 ? 1 : 0);
	}

	FORCEINLINE bool Reserve(index TargetCapacity) {
		return Bytes.Reserve(TargetCapacity);
	}

	[[nodiscard]] FORCEINLINE bool operator==(const str& Other) const {
		if (GetByteLength() != Other.GetByteLength()) {
			return false;
		}
		for (index i = 0; i < GetByteLength(); ++i) {
			if (Bytes[i] != Other.Bytes[i]) {
				return false;
			}
		}
		return true;
	}

	[[nodiscard]] FORCEINLINE bool operator==(const str_view Other) const {
		if (GetByteLength() != Other.GetSize()) {
			return false;
		}
		for (index i = 0; i < GetByteLength(); ++i) {
			if (Bytes[i] != Other[i]) {
				return false;
			}
		}
		return true;
	}

	[[nodiscard]] FORCEINLINE bool operator<(const str_view Other) const {
		return Compare(Other) < 0;
	}

	[[nodiscard]] FORCEINLINE bool operator>(const str_view Other) const {
		return Compare(Other) > 0;
	}

	[[nodiscard]] FORCEINLINE bool operator<=(const str_view Other) const {
		return Compare(Other) <= 0;
	}

	[[nodiscard]] FORCEINLINE bool operator>=(const str_view Other) const {
		return Compare(Other) >= 0;
	}

	FORCEINLINE str operator+(char Char) const& {
		str Result;
		Result.Bytes.EnsureCapacity(GetByteLength() + 2);
		Result = *this;
		Result += Char;
		return Result;
	}

	FORCEINLINE str operator+(char Char) && {
		str Result;
		const index RequiredSize = GetByteLength() + 2;
		if (Bytes.GetCapacity() >= RequiredSize) {
			Result = std::move(*this);
		} else {
			Result.Bytes.EnsureCapacity(RequiredSize);
			Result = *this;
		}
		Result += Char;
		return Result;
	}

	FORCEINLINE str operator+(const str& Other) const& {
		str Result;
		Result.Bytes.EnsureCapacity(GetByteLength() + Other.GetByteLength() + 1);
		Result = *this;
		Result += Other;
		return Result;
	}

	FORCEINLINE str operator+(const str& Other) && {
		str Result;
		const index RequiredSize = GetByteLength() + Other.GetByteLength() + 1;
		if (Bytes.GetCapacity() >= RequiredSize) {
			Result = std::move(*this);
		} else {
			Result.Bytes.EnsureCapacity(RequiredSize);
			Result = *this;
		}
		Result += Other;
		return Result;
	}

	FORCEINLINE str operator+(const str_view Other) const& {
		str Result;
		Result.Bytes.EnsureCapacity(GetByteLength() + Other.GetSize() + 1);
		Result = *this;
		Result += Other;
		return Result;
	}

	FORCEINLINE str operator+(const str_view Other) && {
		str Result;
		const index RequiredSize = GetByteLength() + Other.GetSize() + 1;
		if (Bytes.GetCapacity() >= RequiredSize) {
			Result = std::move(*this);
		} else {
			Result.Bytes.EnsureCapacity(RequiredSize);
			Result = *this;
		}
		Result += Other;
		return Result;
	}

	FORCEINLINE str& operator+=(char Char) {
		auto CharSpot = AppendUninitialized(1);
		CharSpot[0] = Char;
		return *this;
	}

	FORCEINLINE str& operator+=(const str& Other) {
		const index OtherLength = Other.GetByteLength();
		std::memcpy(AppendUninitialized(OtherLength).GetData(), Other.GetData(), OtherLength);
		return *this;
	}

	FORCEINLINE str& operator+=(const str_view Other) {
		std::memcpy(AppendUninitialized(Other.GetSize()).GetData(), Other.GetData(), Other.GetSize());
		return *this;
	}

	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::MurmurHash(GetData(), (s32) GetByteLength());
	}

	FORCEINLINE void Clear(container_clear_type ClearType = container_clear_type::deallocate) {
		Bytes.Clear(ClearType);
	}

	// 0 is equality, >=1 is Lhs greater (bigger chars or longer), <=-1 is less
	FORCEINLINE s32 Compare(str_view Rhs) const {
		for (s32 Index = 0; Index < GetByteLength() && Index < Rhs.GetSize(); ++Index) {
			const s32 Difference = static_cast<s32>(Bytes[Index]) - static_cast<s32>(Rhs[Index]);
			if (Difference) {
				return Difference;
			}
		}
		return GetByteLength() - Rhs.GetSize();
	}

	FORCEINLINE mutable_str_view AppendUninitialized(index AppendSize) {
		if (!AppendSize) {
			return mutable_str_view{};
		}
		const bool HadZeroTerminator = Bytes.GetSize() > 0;
		auto Span = Bytes.AppendUninitialized(AppendSize + !HadZeroTerminator);
		Span[Span.GetSize() - 1] = 0;
		if (!HadZeroTerminator) {
			Span.SliceBack();
		} else {
			Span.ShiftBackward();
		}
		return Span;
	}
};

FORCEINLINE str operator+(char Char, const str& String) {
	return str(Char, String);
}

inline std::ostream& operator<<(std::ostream& Os, const str& String) {
	return Os << String.GetData();
}