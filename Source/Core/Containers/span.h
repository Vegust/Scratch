#pragma once

#include "basic.h"
#include "Utility/hash.h"
#include "array_iter.h"
#include <cstring>

// const non-owning view into str/array/dyn_array/C string
template <typename element_type>
struct span {
private:
	const element_type* Data{nullptr};
	index Size{0};

public:
	// both iterators are const because you can't change span data (for now?) // TODO
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

	template <typename container_type>
		requires(
			container_type::const_iter::Contiguous &&
			std::is_same<
				typename std::remove_const<typename container_type::value_type>::type,
				typename std::remove_const<value_type>::type>::value)
	FORCEINLINE constexpr bool operator==(const container_type& Other) const {
		const value_type* OtherBegin = Other.begin();
		const index OtherSize = Other.end() - OtherBegin;
		if (Size != OtherSize) {
			return false;
		}
		for (int i = 0; i < Size; ++i) {
			if (Data[i] != OtherBegin[i]) {
				return false;
			}
		}
		return true;
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

	[[nodiscard]] FORCEINLINE constexpr span RemoveFirst() const {
		span Result{*this};
		Result.Data += 1;
		Result.Size -= 1;
		return Result;
	}

	[[nodiscard]] FORCEINLINE constexpr span RemoveLast() const {
		span Result{*this};
		Result.Size -= 1;
		return Result;
	}

	// hasher for string-like spans, should result in identical hashes to strings with same data
	// TODO: explicitly make it into one shared function
	template <typename cur_type = value_type>
		requires(std::is_same<typename std::remove_const<cur_type>::type, char>::value)
	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::MurmurHash(GetData(), (s32) GetSize());
	}
};

// deduction guide, sadly does not work for implicit conversions
template <typename container_type>
span(container_type&) -> span<typename container_type::value_type>;