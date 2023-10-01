#pragma once

#include "core_types.h"

#include <iostream>

template <typename allocator_type>
struct allocator_instance {
	NO_UNIQUE_ADDRESS allocator_type mAllocator{};
};

FORCEINLINE static void* MemoryMalloc(u64 Size) {
	return std::malloc(Size);
}

FORCEINLINE static void MemoryFree(void* Ptr) {
	std::free(Ptr);
}

template <typename sub_type>
struct allocator_base {
	constexpr static bool Debug = false;

	FORCEINLINE void* Allocate(u64 Size, u8 Alignment = 8) {
		if constexpr (Debug) {
			auto* Ptr = static_cast<sub_type*>(this)->AllocateImpl(Size);
			std::cout << Ptr << " alloc   " << Size << " bytes" << std::endl;
			return Ptr;
		} else {
			auto* Ptr = static_cast<sub_type*>(this)->AllocateImpl(Size, Alignment);;
			CHECK(Ptr)
			return Ptr;
		}
	}

	FORCEINLINE void Free(void* Ptr) {
		if (!Ptr) {
			return;
		}
		if constexpr (Debug) {
			std::cout << Ptr << " dealloc" << std::endl;
		}
		static_cast<sub_type*>(this)->FreeImpl(Ptr);
	}

	FORCEINLINE bool Expand(void* Ptr, u64 NewSize) {
		return static_cast<sub_type*>(this)->ExpandImpl(Ptr, NewSize);
	}
};

struct malloc_allocator : allocator_base<malloc_allocator> {
	FORCEINLINE void* AllocateImpl(u64 Size, u8 Alignment = 8) {
		return MemoryMalloc(Size);
	}

	FORCEINLINE void FreeImpl(void* Ptr) {
		MemoryFree(Ptr);
	}

	FORCEINLINE bool ExpandImpl(void* Ptr, u64 NewSize) {
		return false;
	}
};