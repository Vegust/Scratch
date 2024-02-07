#pragma once

#include "basic.h"
#include "Hash/hash.h"
#include "array_iter.h"
#include <cstring>

// const non-owning view into str/array/dyn_array/C string
template <typename element_type>
struct span {
private:
	const element_type* Data{nullptr};
	index Size{0};

public:
	// both iterators are const because you can't change span data
	using iter = array_iter<span, iterator_constness::constant>;
	using const_iter = array_iter<span, iterator_constness::constant>;
	using value_type = element_type;

	// defines constexpr analogs to strlen etc
	using traits = std::char_traits<char>;

	span() = default;
	~span() = default;
	span(const span&) = default;
	span(span&&) = default;
	span& operator=(const span&) = default;
	span& operator=(span&&) = default;

	FORCEINLINE constexpr explicit span(const element_type* InData, const index InSize) : Data{InData}, Size{InSize} {
	}

	FORCEINLINE constexpr explicit span(const element_type* Begin, const element_type* End)
		: Data{Begin}, Size{static_cast<index>(End - Begin)} {
	}

	// conversion to span can be implicit
	FORCEINLINE constexpr span(const char* Source)	  // NOLINT(*-explicit-constructor)
		: Data{Source} {
		Size = static_cast<index>(traits::length(Source));
	}

	// conversion to span can be implicit
	template <typename container_type>
		requires(
			container_type::const_iter::Contiguous &&
			std::is_same<
				typename std::remove_const<typename container_type::value_type>::type,
				typename std::remove_const<value_type>::type>::value)
	FORCEINLINE constexpr span(const container_type& Container)	   // NOLINT(*-explicit-constructor)
		: span{Container.begin(), Container.end()} {
	}

	// conversion from span must be explicit because it is most likely a new allocation
	template <typename container_type>
		requires(
			container_type::const_iter::Contiguous &&
			std::is_same<
				typename std::remove_const<typename container_type::value_type>::type,
				typename std::remove_const<value_type>::type>::value)
	FORCEINLINE explicit operator container_type() const {
		if (Size == 0) {
			return {};
		} else {
			return container_type{Data, Size};
		}
	}

	FORCEINLINE constexpr bool operator==(const span& Other) const {
		if (Size != Other.Size) {
			return false;
		}
		for (index i = 0; i < Size; ++i) {
			if (Other[i] != operator[](i)) {
				return false;
			}
		}
		return true;
	}

	FORCEINLINE constexpr const element_type& operator[](const index Index) const {
		return Data[Index];
	}

	[[nodiscard]] FORCEINLINE constexpr const element_type* GetData() const {
		return Data;
	}

	[[nodiscard]] FORCEINLINE constexpr index GetSize() const {
		return Size;
	}

	[[nodiscard]] FORCEINLINE constexpr bool IsEmpty() const {
		return !Data || Size == 0;
	}

	FORCEINLINE constexpr void Clear() {
		Data = nullptr;
		Size = 0;
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

	FORCEINLINE constexpr span SliceFront(index SlicedSize = 1) {
		CHECK(SlicedSize <= Size)
		span Result{*this};
		Result.Data = Data;
		Result.Size = SlicedSize;
		Data += SlicedSize;
		Size -= SlicedSize;
		return Result;
	}

	FORCEINLINE constexpr span SliceBack(index SlicedSize = 1) {
		CHECK(SlicedSize <= Size)
		span Result{*this};
		Result.Data = Data + Size - (Size > 0) - SlicedSize;
		Result.Size = SlicedSize;
		Size -= SlicedSize;
		return Result;
	}

	FORCEINLINE constexpr span& ShiftForward(index Size = 1) {
		CHECK(Data)
		Data += Size;
		return *this;
	}

	FORCEINLINE constexpr span& ShiftBackward(index Size = 1) {
		CHECK(Data)
		Data -= Size;
		return *this;
	}

	// hasher for string-like spans, should result in identical hashes to strings with same data
	// TODO: explicitly make it into one shared function
	template <typename cur_type = value_type>
		requires(std::is_same<typename std::remove_const<cur_type>::type, char>::value)
	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::MurmurHash(GetData(), (s32) GetSize());
	}
};

// NON-const non-owning view into str/array/dyn_array/C string
// NOTE: maybe just copying span and removing const is not the best way to do it?
template <typename element_type>
struct mutable_span {
private:
	element_type* Data{nullptr};
	index Size{0};

public:
	using iter = array_iter<mutable_span, iterator_constness::non_constant>;
	using const_iter = array_iter<mutable_span, iterator_constness::constant>;
	using value_type = element_type;

	// defines constexpr analogs to strlen etc
	using traits = std::char_traits<char>;

	mutable_span() = default;
	~mutable_span() = default;
	mutable_span(const mutable_span&) = default;
	mutable_span(mutable_span&&) = default;
	mutable_span& operator=(const mutable_span&) = default;
	mutable_span& operator=(mutable_span&&) = default;

	FORCEINLINE constexpr explicit mutable_span(element_type* InData, const index InSize) : Data{InData}, Size{InSize} {
	}

	FORCEINLINE constexpr explicit mutable_span(element_type* Begin, element_type* End)
		: Data{Begin}, Size{static_cast<index>(End - Begin)} {
	}

	// conversion to span can be implicit
	FORCEINLINE constexpr mutable_span(char* Source)	// NOLINT(*-explicit-constructor)
		: Data{Source} {
		Size = static_cast<index>(traits::length(Source));
	}

	// conversion to span can be implicit
	template <typename container_type>
		requires(
			container_type::const_iter::Contiguous &&
			std::is_same<
				typename std::remove_const<typename container_type::value_type>::type,
				typename std::remove_const<value_type>::type>::value)
	FORCEINLINE constexpr mutable_span(container_type& Container)	 // NOLINT(*-explicit-constructor)
		: mutable_span{Container.begin(), Container.end()} {
	}

	// conversion from span must be explicit because it is most likely a new allocation
	template <typename container_type>
		requires(
			container_type::const_iter::Contiguous &&
			std::is_same<
				typename std::remove_const<typename container_type::value_type>::type,
				typename std::remove_const<value_type>::type>::value)
	FORCEINLINE explicit operator container_type() const {
		if (Size == 0) {
			return {};
		} else {
			return container_type{Data, Size};
		}
	}

	FORCEINLINE constexpr bool operator==(const mutable_span& Other) const {
		if (Size != Other.Size) {
			return false;
		}
		for (index i = 0; i < Size; ++i) {
			if (Other[i] != operator[](i)) {
				return false;
			}
		}
		return true;
	}

	FORCEINLINE constexpr element_type& operator[](const index Index) {
		return Data[Index];
	}

	FORCEINLINE constexpr const element_type& operator[](const index Index) const {
		return Data[Index];
	}

	[[nodiscard]] FORCEINLINE constexpr element_type* GetData() {
		return Data;
	}

	[[nodiscard]] FORCEINLINE constexpr const element_type* GetData() const {
		return Data;
	}

	[[nodiscard]] FORCEINLINE constexpr index GetSize() const {
		return Size;
	}

	[[nodiscard]] FORCEINLINE constexpr bool IsEmpty() const {
		return !Data || Size == 0;
	}

	FORCEINLINE constexpr void Clear() {
		Data = nullptr;
		Size = 0;
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

	FORCEINLINE constexpr mutable_span SliceFront(index SlicedSize = 1) {
		CHECK(SlicedSize <= Size)
		mutable_span Result{*this};
		Result.Data = Data;
		Result.Size = SlicedSize;
		Data += SlicedSize;
		Size -= SlicedSize;
		return Result;
	}

	FORCEINLINE constexpr mutable_span SliceBack(index SlicedSize = 1) {
		CHECK(SlicedSize <= Size)
		mutable_span Result{*this};
		Result.Data = Data + Size - (Size > 0) - SlicedSize;
		Result.Size = SlicedSize;
		Size -= SlicedSize;
		return Result;
	}

	FORCEINLINE constexpr mutable_span& ShiftForward(index Size = 1) {
		CHECK(Data)
		Data += Size;
		return *this;
	}

	FORCEINLINE constexpr mutable_span& ShiftBackward(index Size = 1) {
		CHECK(Data)
		Data -= Size;
		return *this;
	}

	// hasher for string-like spans, should result in identical hashes to strings with same data
	// TODO: explicitly make it into one shared function
	template <typename cur_type = value_type>
		requires(std::is_same<typename std::remove_const<cur_type>::type, char>::value)
	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::MurmurHash(GetData(), (s32) GetSize());
	}
};