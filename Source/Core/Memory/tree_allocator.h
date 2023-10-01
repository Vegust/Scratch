#pragma once

#include "allocator_base.h"

struct tree_allocator : allocator_base<tree_allocator> {
	void* AllocateImpl(u64 Size, u8 Alignment = 8);

	void FreeImpl(void* Ptr);

	bool ExpandImpl(void* Ptr, u64 NewSize);
	
	// For measuring memory in tests/benchmarks
	static void ClearStaticImpl();
};