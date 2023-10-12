#pragma once

#include "core_types.h"
#include "array_iter.h"
#include <cstring>

// const non-owning view into str/array/dyn_array/C string
template <typename element_type>
struct span {
	const element_type* mData{nullptr};
	index mSize{0};

	// both iterators are const because you can't change span data (for now?) // TODO
	using iter = array_iter<span, true>;
	using const_iter = array_iter<span, true>;
	using value_type = element_type;

	span() = default;
	~span() = default;
	span(const span&) = default;
	span(span&&) = default;
	span& operator=(const span&) = default;
	span& operator=(span&&) = default;

	FORCEINLINE constexpr explicit span(const element_type* Source, const index Count)
		: mData{Source}, mSize{Count} {
	}

	FORCEINLINE constexpr explicit span(const element_type* Begin, const element_type* End)
		: mData{Begin}, mSize{static_cast<index>(End - Begin)} {
	}

	// conversion to span can be implicit
	FORCEINLINE constexpr span(const char* Source)	  // NOLINT(*-explicit-constructor)
		: mData{Source} {
		mSize = static_cast<index>(strlen(Source));
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
		if (mSize == 0) {
			return {};
		} else {
			return container_type{mData, mSize};
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
		if (mSize != OtherSize) {
			return false;
		}
		for (int i = 0; i < mSize; ++i) {
			if (mData[i] != OtherBegin[i]) {
				return false;
			}
		}
		return true;
	}

	FORCEINLINE constexpr bool operator==(const span& Other) const {
		if (mSize != Other.mSize) {
			return false;
		}
		for (index i = 0; i < mSize; ++i) {
			if (Other[i] != operator[](i)) {
				return false;
			}
		}
		return true;
	}

	FORCEINLINE constexpr const element_type& operator[](const index Index) const {
		return mData[Index];
	}

	FORCEINLINE constexpr const value_type* Data() const {
		return mData;
	}

	[[nodiscard]] FORCEINLINE constexpr index Size() const {
		return mSize;
	}

	[[nodiscard]] FORCEINLINE constexpr bool Empty() const {
		return !mData || mSize == 0;
	}

	FORCEINLINE constexpr void Clear() {
		mData = nullptr;
		mSize = 0;
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

	// hasher for string-like spans, should result in identical hashes to strings with same data
	template <typename cur_type = value_type>
		requires(std::is_same<typename std::remove_const<cur_type>::type, char>::value)
	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::MurmurHash(Data(), (s32) mSize);
	}
};