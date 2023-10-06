#pragma once

#include "allocator_base.h"

struct tree_allocator : allocator_base<tree_allocator> {
	FORCEINLINE void* AllocateImpl(u64 Size, u8 Alignment = 8) {
		return StaticAllocateImpl(Size, Alignment);
	}

	FORCEINLINE void FreeImpl(void* Ptr) {
		StaticFreeImpl(Ptr);
	}

	FORCEINLINE bool ExpandImpl(void* Ptr, u64 NewSize) {
		return StaticExpandImpl(Ptr, NewSize);
	}

	static void* StaticAllocateImpl(u64 Size, u8 Alignment = 8);

	static void StaticFreeImpl(void* Ptr);

	static bool StaticExpandImpl(void* Ptr, u64 NewSize);

	// For measuring memory in tests/benchmarks
	static void ClearStaticImpl();
};