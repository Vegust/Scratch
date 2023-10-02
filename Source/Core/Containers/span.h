#pragma once

#include "core_types.h"
#include "array_iter.h"

// const non-owning view into str/array/dyn_array/C string
template <typename element_type>
struct span {
	const element_type* mData;
	index_type mSize;

	using iter = array_iter<span, false>;
	using value_type = element_type;
	using const_iter = array_iter<span, true>;

	span() = default;
	~span() = default;

	FORCEINLINE constexpr explicit span(const element_type* Source, const index_type Count)
		: mData{Source}, mSize{Count} {
	}

	FORCEINLINE constexpr explicit span(const element_type* Begin, const element_type* End)
		: mData{Begin}, mSize{static_cast<index_type>(End - Begin)} {
	}

	FORCEINLINE constexpr span(const char* Source) : mData{Source} {
		mSize = static_cast<index_type>(strlen(Source));
	}

	// conversion to span can be implicit
	template <typename container_type> requires(
			container_type::const_iter::Contiguous &&
			std::is_same<typename container_type::value_type, value_type>::value)
	FORCEINLINE constexpr span(const container_type& Container)
		: span{Container.begin(), Container.end()} {
	}

	// conversion from span must be explicit because it is most likely a new allocation
	template <typename container_type>
		requires(
			container_type::const_iter::Contiguous &&
			std::is_same<typename container_type::value_type, value_type>::value)
	FORCEINLINE explicit operator container_type() {
		return container_type{mData, mSize};
	}

	FORCEINLINE constexpr span(const span& Source) : mData{Source.mData}, mSize{Source.mSize} {
	}

	FORCEINLINE constexpr span(span&& Source) : mData{Source.mData}, mSize{Source.mSize} {
		Source.Clear();
	}

	FORCEINLINE constexpr span& operator=(const span& Other) const {
		mData = Other.mData;
		mSize = Other.mSize;
	}

	FORCEINLINE constexpr span& operator=(span&& Other) const {
		mData = Other.mData;
		mSize = Other.mSize;
		Other.Clear();
	}

	FORCEINLINE constexpr bool operator==(const span& Other) const {
		if (mSize != Other.mSize) {
			return false;
		}
		for (index_type i = 0; i < mSize; ++i) {
			if (Other[i] != operator[](i)) {
				return false;
			}
		}
		return true;
	}

	FORCEINLINE constexpr const element_type& operator[](const index_type Index) const {
		return mData[Index];
	}

	FORCEINLINE constexpr value_type* Data() const {
		return mData;
	}

	FORCEINLINE constexpr index_type Size() const {
		return mSize;
	}

	FORCEINLINE constexpr bool Empty() const {
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
};