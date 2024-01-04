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
	static constexpr bool MemcopyRelocatable = true;

	using array_type = dyn_array<char_type, default_allocator, StackSize>;
	using iter = array_type::iter;
	using const_iter = array_type::const_iter;

private:
	array_type Bytes{};

public:
	// defines constexpr analogs to strlen etc
	using traits = std::char_traits<char_type>;

	str() = default;

	str(const char* Characters) {	 // NOLINT(*-explicit-constructor)
		const index Length = traits::length(Characters);
		Bytes.EnsureCapacity(Length + 1);
		std::memcpy(Bytes.GetData(), Characters, Length + 1);
		Bytes.OverwriteSize(Length + 1);
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

	template <integral integral_type>
	integral_type GetNumber() {
		// TODO
		return 0;
	}

	template <fractional float_type>
	float_type GetNumber() {
		// TODO
		return 0.f;
	}

	FORCEINLINE str_view GetIntegerString(str_view String) {
		//		str_view Start = EatSpaces(String);
		//		index NumberLength = 0;
		//		while (NumberLength < Start.Size() && )
		return {};
	}

	str_view EatSpaces(str_view String) {
		if (String.IsEmpty()) {
			return {};
		}
		const char* NewStart = String.GetData();
		const char* StringEnd = String.GetData() + String.GetSize();
		while (IsSpace(*NewStart) && NewStart < StringEnd) {
			++NewStart;
		}
		if (NewStart == StringEnd) {
			return {};
		}
		return str_view{NewStart, StringEnd};
	}

	FORCEINLINE bool IsSpace(char Character) {
		return Character == ' ' || Character == '\t' || Character == '\n' || Character == '\r';
	}

	FORCEINLINE bool IsNumber(char Character) {
		return '0' <= Character && Character <= '9';
	}

	str_view GetLine(str_view Previous = {}) {
		if (IsEmpty()) {
			return {};
		}
		const char* LineStart = GetRaw();
		if (Previous.GetData() != nullptr) {
			LineStart = Previous.GetData() + Previous.GetSize();
			if (*LineStart == '\n') {
				++LineStart;
			} else {
				return {};
			}
		}
		const char* LineEnd = LineStart;
		const char* StringEnd = GetData() + GetByteLength();
		while (*LineEnd != '\n' && LineEnd < StringEnd) {
			++LineEnd;
		}
		return str_view{LineStart, LineEnd};
	}

	FORCEINLINE str& operator=(str&& Other) noexcept {
		Bytes = std::move(Other.Bytes);
		return *this;
	}

	FORCEINLINE str& operator=(const str& Other) {
		Bytes = Other.Bytes;
		return *this;
	}

	FORCEINLINE str& operator=(const char* Characters) {
		Bytes.Clear(container_clear_type::dont_deallocate);
		if (!Characters) {
			return *this;
		}
		const index Length = traits::length(Characters);
		Bytes.EnsureCapacity(Length + 1);
		std::memcpy(Bytes.GetData(), Characters, Length + 1);
		Bytes.OverwriteSize(Length + 1);
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

	[[nodiscard]] FORCEINLINE bool operator<(const str& Other) const {
		return Strcmp(GetRaw(), Other.GetRaw()) < 0;
	}

	[[nodiscard]] FORCEINLINE bool operator>(const str& Other) const {
		return Strcmp(GetRaw(), Other.GetRaw()) > 0;
	}

	[[nodiscard]] FORCEINLINE bool operator<=(const str& Other) const {
		return Strcmp(GetRaw(), Other.GetRaw()) <= 0;
	}

	[[nodiscard]] FORCEINLINE bool operator>=(const str& Other) const {
		return Strcmp(GetRaw(), Other.GetRaw()) >= 0;
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

	FORCEINLINE str operator+(const char* Characters) const& {
		str Result;
		Result.Bytes.EnsureCapacity(GetByteLength() + traits::length(Characters) + 1);
		Result = *this;
		Result += Characters;
		return Result;
	}

	FORCEINLINE str operator+(const char* Characters) && {
		str Result;
		const index RequiredSize = GetByteLength() + traits::length(Characters) + 1;
		if (Bytes.GetCapacity() >= RequiredSize) {
			Result = std::move(*this);
		} else {
			Result.Bytes.EnsureCapacity(RequiredSize);
			Result = *this;
		}
		Result += Characters;
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

	FORCEINLINE str& operator+=(const char* Chars) {
		if (!Chars || Chars[0] == 0) {
			return *this;
		}
		const index LhsLen = GetByteLength();
		if (LhsLen == 0) {
			*this = Chars;
			return *this;
		}
		const index RhsLen = traits::length(Chars);
		Bytes.EnsureCapacity(LhsLen + RhsLen + 1);
		memcpy(GetRaw() + LhsLen, Chars, RhsLen);
		Bytes[LhsLen + RhsLen] = 0;
		Bytes.OverwriteSize(LhsLen + RhsLen + 1);
		return *this;
	}

	FORCEINLINE str Substr(index Begin, index End) const {
		return str{GetRaw() + Begin, End - Begin};
	}

	FORCEINLINE index FindLastOf(char Char) const {
		index LastIndex = InvalidIndex;
		for (int i = 0; i < GetByteLength(); ++i) {
			if (Bytes[i] == Char) {
				LastIndex = i;
			}
		}
		return LastIndex;
	}

	FORCEINLINE index Find(const char* Substring, index StartIndex = 0) const {
		if (!Substring || IsEmpty() || Bytes.GetSize() <= StartIndex) {
			return InvalidIndex;
		}
		const char* RawData = GetRaw() + StartIndex;
		char Current = *RawData++;
		const char Start = *Substring;
		while (Current) {
			if (Current == Start && !Strcmp<true>(RawData, Substring + 1)) {
				return RawData - 1 - GetRaw();
			}
			Current = *RawData++;
		}
		return InvalidIndex;
	}

	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::MurmurHash(GetData(), (s32) GetByteLength());
	}

	// NOTE: I don't like this function
	template <bool SubstringCmp = false>
	[[nodiscard]] FORCEINLINE s32 Strcmp(const char* Lhs, const char* Rhs) const {
		for (;;) {
			u8 LhsChar = static_cast<u8>(*Lhs++);
			u8 RhsChar = static_cast<u8>(*Rhs++);
			if (LhsChar == RhsChar) {
				if (LhsChar) {
					continue;
				}
				return 0;
			} else if (LhsChar != 0 && RhsChar != 0) {
				if (s32 Diff = LhsChar - RhsChar) {
					return Diff;
				}
			} else {
				if constexpr (SubstringCmp) {
					return 0;
				} else {
					return LhsChar - RhsChar;
				}
			}
		}
	}

	FORCEINLINE void Clear(container_clear_type ClearType = container_clear_type::deallocate) {
		Bytes.Clear(ClearType);
	}

	FORCEINLINE void OverwriteSize(const index NewSize) {
		Bytes.OverwriteSize(NewSize);
	}
};

FORCEINLINE str operator+(char Char, const str& String) {
	return str(Char, String);
}

inline std::ostream& operator<<(std::ostream& Os, const str& String) {
	return Os << String.GetData();
}