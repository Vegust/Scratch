#pragma once

#include "basic.h"
#include "Logger/logger.h"

#include <iostream>

template <typename allocator_type>
struct allocator_instance {
	NO_UNIQUE_ADDRESS allocator_type Allocator{};
	
	void SetAllocator(allocator_type& Allocator) {
		Allocator = Allocator;
	}
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
			logger::Log("%d dealloc", (u64)Ptr);
		}
		static_cast<sub_type*>(this)->FreeImpl(Ptr);
	}

	FORCEINLINE bool Expand(void* Ptr, u64 NewSize) {
		return static_cast<sub_type*>(this)->ExpandImpl(Ptr, NewSize);
	}

	FORCEINLINE static void* StaticAllocate(u64 Size, u8 Alignment = 8) {
		if constexpr (Debug) {
			auto* Ptr = sub_type::StaticAllocateImpl(Size);
			std::cout << Ptr << " alloc   " << Size << " bytes" << std::endl;
			return Ptr;
		} else {
			auto* Ptr = sub_type::StaticAllocateImpl(Size, Alignment);;
			CHECK(Ptr)
			return Ptr;
		}
	}

	FORCEINLINE static void StaticFree(void* Ptr) {
		if (!Ptr) {
			return;
		}
		if constexpr (Debug) {
			logger::Log("%d dealloc", (u64)Ptr);
		}
		sub_type::StaticFreeImpl(Ptr);
	}

	FORCEINLINE static bool StaticExpand(void* Ptr, u64 NewSize) {
		return sub_type::StaticExpandImpl(Ptr, NewSize);
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