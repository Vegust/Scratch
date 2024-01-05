#pragma once

#include "basic.h"
#include "Containers/array.h"
#include "Containers/dyn_array.h"
#include "Core/Utility/hash.h"
#include "Core/Containers/span.h"

#include <iostream>

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
		Bytes.EnsureCapacity(Length + 1);
		std::memcpy(Bytes.GetData(), Characters, Length);
		Bytes[Length] = 0;
		Bytes.OverwriteSize(Length + 1);
	}

	str(const str& Other) : Bytes(Other.Bytes) {
	}

	str(str&& Other) : Bytes(std::move(Other.Bytes)) {
	}

	str(char Char, const str& String) {
		const index RequiredSize = String.GetByteLength() + 2;
		Bytes.EnsureCapacity(RequiredSize);
		Bytes.OverwriteSize(String.Bytes.GetSize() + 1);
		std::memcpy(GetRaw() + 1, String.GetRaw(), String.GetByteLength());
		Bytes[String.GetByteLength() + 1] = 0;
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
		if (Other.IsEmpty()) {
			return *this;
		}
		Bytes.EnsureCapacity(Other.GetSize() + 1);
		std::memcpy(Bytes.GetData(), Other.GetData(), Other.GetSize());
		Bytes[Other.GetSize()] = 0;
		Bytes.OverwriteSize(Other.GetSize() + 1);
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
		const index LhsLen = GetByteLength();
		if (LhsLen == 0) {
			Bytes.EnsureCapacity(2);
			Bytes[0] = Char;
			Bytes[1] = 0;
			Bytes.OverwriteSize(2);
			return *this;
		}
		Bytes.EnsureCapacity(LhsLen + 2);
		Bytes[LhsLen] = Char;
		Bytes[LhsLen + 1] = 0;
		Bytes.OverwriteSize(LhsLen + 2);
		return *this;
	}

	FORCEINLINE str& operator+=(const str& Other) {
		const index LhsLen = GetByteLength();
		if (LhsLen == 0) {
			*this = Other;
			return *this;
		}
		const index RhsLen = Other.GetByteLength();
		if (RhsLen == 0) {
			return *this;
		}
		Bytes.EnsureCapacity(LhsLen + RhsLen + 1);
		memcpy(GetRaw() + LhsLen, Other.GetRaw(), RhsLen);
		Bytes[LhsLen + RhsLen] = 0;
		Bytes.OverwriteSize(LhsLen + RhsLen + 1);
		return *this;
	}

	FORCEINLINE str& operator+=(const str_view Other) {
		if (Other.IsEmpty()) {
			return *this;
		}
		const index LhsLen = GetByteLength();
		if (LhsLen == 0) {
			*this = Other;
			return *this;
		}
		const index RhsLen = Other.GetSize();
		Bytes.EnsureCapacity(LhsLen + RhsLen + 1);
		memcpy(GetRaw() + LhsLen, Other.GetData(), RhsLen);
		Bytes[LhsLen + RhsLen] = 0;
		Bytes.OverwriteSize(LhsLen + RhsLen + 1);
		return *this;
	}

	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::MurmurHash(GetData(), (s32) GetByteLength());
	}

	FORCEINLINE void Clear(container_clear_type ClearType = container_clear_type::deallocate) {
		Bytes.Clear(ClearType);
	}

	FORCEINLINE void OverwriteSize(const index NewSize) {
		Bytes.OverwriteSize(NewSize);
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
};

FORCEINLINE str operator+(char Char, const str& String) {
	return str(Char, String);
}

inline std::ostream& operator<<(std::ostream& Os, const str& String) {
	return Os << String.GetData();
}