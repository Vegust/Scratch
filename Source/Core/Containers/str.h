#pragma once

#include "Containers/array.h"
#include "Containers/dyn_array.h"
#include "core_types.h"
#include "hash.h"
#include "span.h"

#include <iostream>

using str_view = span<char>;

struct str final : trait_memcopy_relocatable {
	using char_type = char;
	using value_type = char;
	constexpr static index StackSize = 14;
	using array_type = dyn_array<char_type, default_allocator, StackSize>;
	using iter = array_type::iter;
	using const_iter = array_type::const_iter;

	array_type mChars{};

	str() = default;

	str(const char* Characters) {	 // NOLINT(*-explicit-constructor)
		const index Length = strlen(Characters);
		mChars.EnsureCapacity(Length + 1);
		std::memcpy(mChars.Data(), Characters, Length + 1);
		mChars.mSize = Length + 1;
	}

	str(const char* Characters, index Length) {
		mChars.EnsureCapacity(Length + 1);
		std::memcpy(mChars.Data(), Characters, Length + 1);
		mChars.mSize = Length + 1;
	}

	str(const str& Other) : mChars(Other.mChars) {
	}

	str(str&& Other) : mChars(std::move(Other.mChars)) {
	}

	template <integral integral_type>
	explicit str(integral_type IntegralNumber) {
		// clang-format off
		constexpr array<char, 19> LookupTable = 
		{'9','8','7','6','5','4','3','2','1','0','1','2','3','4','5','6','7','8','9'};
		constexpr index ZeroIndex = 9;
		array<char, 20> Buffer{}; // Should be enough for negative 64 bit int
		// clang-format on
		char* NumberStart = Buffer.end();
		bool Signed = IntegralNumber < 0;
		index NumChars = Signed ? 1 : 0;
		do {
			*--NumberStart = LookupTable[ZeroIndex + (IntegralNumber % 10)];
			IntegralNumber /= 10;
			++NumChars;
		} while (IntegralNumber);
		if (Signed) {
			*--NumberStart = '-';
		}
		mChars.EnsureCapacity(NumChars + 1);
		std::memcpy(mChars.Data(), NumberStart, NumChars);
		mChars[NumChars] = 0;
		mChars.mSize = NumChars + 1;
	}

	//	template<floating_point float_type>
	//	explicit str(float_type FloatingNumber) {
	//		TODO:
	//	}

	FORCEINLINE str& operator=(str&& Other) noexcept {
		mChars = std::move(Other.mChars);
		return *this;
	}

	FORCEINLINE str& operator=(const str& Other) {
		mChars = Other.mChars;
		return *this;
	}

	FORCEINLINE str& operator=(const char* Characters) {
		mChars.Clear(false);
		if (!Characters) {
			return *this;
		}
		const index Length = strlen(Characters);
		mChars.EnsureCapacity(Length + 1);
		std::memcpy(mChars.Data(), Characters, Length + 1);
		mChars.mSize = Length + 1;
		return *this;
	}

	FORCEINLINE char_type* Data() {
		return mChars.Data();
	}

	FORCEINLINE char* Raw() {
		return (char*) mChars.Data();
	}

	[[nodiscard]] FORCEINLINE const char* Raw() const {
		return (char*) mChars.Data();
	}

	[[nodiscard]] FORCEINLINE const char_type* Data() const {
		return mChars.Data();
	}

	[[nodiscard]] FORCEINLINE index Length() const {
		return mChars.Size() ? mChars.Size() - 1 : 0;
	}

	[[nodiscard]] FORCEINLINE index Empty() const {
		return Length() == 0;
	}

	FORCEINLINE const char_type& operator[](const index Index) const {
		return mChars[Index];
	}

	FORCEINLINE char_type& At(index Position) {
		return mChars[Position];
	}

	[[nodiscard]] FORCEINLINE const char_type& At(index Position) const {
		return mChars[Position];
	}

	FORCEINLINE iter begin() {
		return mChars.begin();
	}

	FORCEINLINE iter end() {
		return mChars.end() - (Length() > 0 ? 1 : 0);
	}

	[[nodiscard]] FORCEINLINE const_iter begin() const {
		return mChars.begin();
	}

	[[nodiscard]] FORCEINLINE const_iter end() const {
		return mChars.end() - (Length() > 0 ? 1 : 0);
	}

	[[nodiscard]] FORCEINLINE bool operator==(const str& Other) const {
		if (Length() != Other.Length()) {
			return false;
		}
		for (index i = 0; i < Length(); ++i) {
			if (mChars[i] != Other.mChars[i]) {
				return false;
			}
		}
		return true;
	}

	[[nodiscard]] FORCEINLINE bool operator<(const str& Other) const {
		return Strcmp(Raw(), Other.Raw()) < 0;
	}

	[[nodiscard]] FORCEINLINE bool operator>(const str& Other) const {
		return Strcmp(Raw(), Other.Raw()) > 0;
	}

	[[nodiscard]] FORCEINLINE bool operator<=(const str& Other) const {
		return Strcmp(Raw(), Other.Raw()) <= 0;
	}

	[[nodiscard]] FORCEINLINE bool operator>=(const str& Other) const {
		return Strcmp(Raw(), Other.Raw()) >= 0;
	}

	FORCEINLINE str operator+(char Char) const& {
		str Result;
		Result.mChars.EnsureCapacity(Length() + 2);
		Result = *this;
		Result += Char;
		return Result;
	}

	FORCEINLINE str operator+(char Char) && {
		str Result;
		const index RequiredSize = Length() + 2;
		if (mChars.mCapacity >= RequiredSize) {
			Result = std::move(*this);
		} else {
			Result.mChars.EnsureCapacity(RequiredSize);
			Result = *this;
		}
		Result += Char;
		return Result;
	}

	FORCEINLINE str operator+(const str& Other) const& {
		str Result;
		Result.mChars.EnsureCapacity(Length() + Other.Length() + 1);
		Result = *this;
		Result += Other;
		return Result;
	}

	FORCEINLINE str operator+(const str& Other) && {
		str Result;
		const index RequiredSize = Length() + Other.Length() + 1;
		if (mChars.mCapacity >= RequiredSize) {
			Result = std::move(*this);
		} else {
			Result.mChars.EnsureCapacity(RequiredSize);
			Result = *this;
		}
		Result += Other;
		return Result;
	}

	FORCEINLINE str operator+(const char* Characters) const& {
		str Result;
		Result.mChars.EnsureCapacity(Length() + strlen(Characters) + 1);
		Result = *this;
		Result += Characters;
		return Result;
	}

	FORCEINLINE str operator+(const char* Characters) && {
		str Result;
		const index RequiredSize = Length() + strlen(Characters) + 1;
		if (mChars.mCapacity >= RequiredSize) {
			Result = std::move(*this);
		} else {
			Result.mChars.EnsureCapacity(RequiredSize);
			Result = *this;
		}
		Result += Characters;
		return Result;
	}

	FORCEINLINE str& operator+=(char Char) {
		const index LhsLen = Length();
		if (LhsLen == 0) {
			mChars.EnsureCapacity(2);
			mChars[0] = Char;
			mChars[1] = 0;
			mChars.mSize = 2;
			return *this;
		}
		mChars.EnsureCapacity(LhsLen + 2);
		mChars[LhsLen] = Char;
		mChars[LhsLen + 1] = 0;
		mChars.mSize = LhsLen + 2;
		return *this;
	}

	FORCEINLINE str& operator+=(const str& Other) {
		const index LhsLen = Length();
		if (LhsLen == 0) {
			*this = Other;
			return *this;
		}
		const index RhsLen = Other.Length();
		if (RhsLen == 0) {
			return *this;
		}
		mChars.EnsureCapacity(LhsLen + RhsLen + 1);
		memcpy(Raw() + LhsLen, Other.Raw(), RhsLen);
		mChars[LhsLen + RhsLen] = 0;
		mChars.mSize = LhsLen + RhsLen + 1;
		return *this;
	}

	FORCEINLINE str& operator+=(const char* Chars) {
		if (!Chars || Chars[0] == 0) {
			return *this;
		}
		const index LhsLen = Length();
		if (LhsLen == 0) {
			*this = Chars;
			return *this;
		}
		const index RhsLen = strlen(Chars);
		mChars.EnsureCapacity(LhsLen + RhsLen + 1);
		memcpy(Raw() + LhsLen, Chars, RhsLen);
		mChars[LhsLen + RhsLen] = 0;
		mChars.mSize = LhsLen + RhsLen + 1;
		return *this;
	}

	FORCEINLINE str Substr(index Begin, index End) const {
		return str{Raw() + Begin, End - Begin};
	}

	FORCEINLINE index FindLastOf(char Char) const {
		index LastIndex = InvalidIndex;
		for (int i = 0; i < Length(); ++i) {
			if (mChars[i] == Char) {
				LastIndex = i;
			}
		}
		return LastIndex;
	}

	FORCEINLINE index Find(const char* Substring, index StartIndex = 0) const {
		if (!Substring || Empty() || mChars.mSize <= StartIndex) {
			return InvalidIndex;
		}
		const char* RawData = Raw() + StartIndex;
		char Current = *RawData++;
		const char Start = *Substring;
		while (Current) {
			if (Current == Start && !Strcmp<true>(RawData, Substring + 1)) {
				return RawData - 1 - Raw();
			}
			Current = *RawData++;
		}
		return InvalidIndex;
	}

	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::MurmurHash(Data(), (s32) Length());
	}

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

	FORCEINLINE void Clear(bool Deallocate = true) {
		mChars.Clear(Deallocate);
	}
};

FORCEINLINE str operator+(char Char, const str& String) {
	str Result;
	const index RequiredSize = String.Length() + 2;
	Result.mChars.EnsureCapacity(RequiredSize);
	Result.mChars.mSize = String.mChars.mSize + 1;
	std::memcpy(Result.Raw() + 1, String.Raw(), String.Length());
	Result.mChars[String.Length() + 1] = 0;
	Result.mChars[0] = Char;
	return Result;
}

inline std::ostream& operator<<(std::ostream& Os, const str& String) {
	return Os << String.mChars.Data();
}