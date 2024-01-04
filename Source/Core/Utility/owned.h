#pragma once

#include "basic.h"
#include "Memory/memory.h"

template <typename owned_type>
struct owned {
private:
	owned_type* Pointer = nullptr;

public:
	FORCEINLINE owned() = default;
	owned(const owned&) = delete;
	owned& operator=(const owned&) = delete;

	FORCEINLINE ~owned() {
		StaticFree(Pointer);
	}

	FORCEINLINE explicit owned(owned_type* Pointer) {
		Pointer = Pointer;
	}

	template <typename child_type>
		requires(std::is_base_of_v<owned_type, child_type> && !std::is_same_v<owned_type, child_type>)
	FORCEINLINE explicit owned(child_type* Pointer) : owned() {
		Pointer = Pointer;
	}

	FORCEINLINE owned& operator=(owned_type* InPointer) {
		if (Pointer == InPointer) {
			return *this;
		}
		StaticFree(Pointer);
		Pointer = InPointer;
		return *this;
	}

	template <typename child_type>
		requires(std::is_base_of_v<owned_type, child_type> && !std::is_same_v<owned_type, child_type>)
	FORCEINLINE owned& operator=(child_type* Pointer) {
		if (Pointer == Pointer) {
			return *this;
		}
		StaticFree(Pointer);
		Pointer = Pointer;
		return *this;
	}

	FORCEINLINE owned(owned&& Other) : Pointer{Other.Pointer} {
		Other.Pointer = nullptr;
	}

	FORCEINLINE owned& operator=(owned&& Other) {
		if (Pointer == Other.Pointer) {
			return *this;
		}
		StaticFree(Pointer);
		Pointer = Other.Pointer;
		Other.Pointer = nullptr;
		return *this;
	}

	FORCEINLINE bool operator==(const owned_type* InPointer) const {
		return Pointer == InPointer;
	}

	FORCEINLINE bool operator!=(const owned_type* InPointer) const {
		return Pointer != InPointer;
	}

	FORCEINLINE bool operator==(const owned& Other) const {
		return Pointer == Other.Pointer;
	}

	FORCEINLINE bool operator!=(const owned& Other) const {
		return Pointer != Other.Pointer;
	}

	const owned_type* operator->() const {
		return Pointer;
	}

	owned_type* operator->() {
		return Pointer;
	}

	owned_type& operator*() noexcept {
		return *Pointer;
	}

	const owned_type& operator*() const {
		return *Pointer;
	}

	FORCEINLINE bool Valid() const {
		return Pointer != nullptr;
	}
};
