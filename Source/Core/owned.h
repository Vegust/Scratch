#pragma once

#include "core_types.h"
#include "Memory/memory.h"

template <typename owned_type>
struct owned {
	owned_type* mPointer = nullptr;

	FORCEINLINE owned() = default;
	owned(const owned&) = delete;
	owned& operator=(const owned&) = delete;

	FORCEINLINE ~owned() {
		StaticFree(mPointer);
	}

	FORCEINLINE explicit owned(owned_type* Pointer) {
		mPointer = Pointer;
	}

	template <typename child_type>
		requires(std::is_base_of_v<owned_type, child_type> && !std::is_same_v<owned_type, child_type>)
	FORCEINLINE explicit owned(child_type* Pointer) : owned() {
		mPointer = Pointer;
	}

	FORCEINLINE owned& operator=(owned_type* Pointer) {
		if (mPointer == Pointer) {
			return *this;
		}
		StaticFree(mPointer);
		mPointer = Pointer;
		return *this;
	}

	template <typename child_type>
		requires(std::is_base_of_v<owned_type, child_type> && !std::is_same_v<owned_type, child_type>)
	FORCEINLINE owned& operator=(child_type* Pointer) {
		if (mPointer == Pointer) {
			return *this;
		}
		StaticFree(mPointer);
		mPointer = Pointer;
		return *this;
	}

	FORCEINLINE owned(owned&& Other) : mPointer{Other.mPointer} {
		Other.mPointer = nullptr;
	}

	FORCEINLINE owned& operator=(owned&& Other) {
		if (mPointer == Other.mPointer) {
			return *this;
		}
		StaticFree(mPointer);
		mPointer = Other.mPointer;
		Other.mPointer = nullptr;
		return *this;
	}

	FORCEINLINE bool operator==(const owned_type* Pointer) const {
		return mPointer == Pointer;
	}

	FORCEINLINE bool operator!=(const owned_type* Pointer) const {
		return mPointer != Pointer;
	}

	FORCEINLINE bool operator==(const owned& Other) const {
		return mPointer == Other.mPointer;
	}

	FORCEINLINE bool operator!=(const owned& Other) const {
		return mPointer != Other.mPointer;
	}

	const owned_type* operator->() const {
		return mPointer;
	}

	owned_type* operator->() {
		return mPointer;
	}

	owned_type& operator*() noexcept {
		return *mPointer;
	}

	const owned_type& operator*() const {
		return *mPointer;
	}

	FORCEINLINE bool Valid() const {
		return mPointer != nullptr;
	}
};
