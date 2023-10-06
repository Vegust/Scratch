#pragma once

#include "allocator_base.h"
#include "tree_allocator.h"

using default_allocator = tree_allocator;

FORCEINLINE void* StaticAlloc(u64 Size, u8 Alignment = 8) {
	default_allocator::StaticAllocate(Size, Alignment);
}

FORCEINLINE void StaticFree(void* Ptr) {
	default_allocator ::StaticFree(Ptr);
}

template <typename new_type, typename... arg_types>
FORCEINLINE new_type* StaticNew(arg_types&&... Args) {
	void* Ptr = default_allocator::StaticAllocate(sizeof(new_type));
	return new (Ptr) new_type(std::forward<arg_types>(Args)...);
}

template <typename deleted_type>
FORCEINLINE void StaticDelete(deleted_type* Ptr) {
	CHECK(Ptr)
	Ptr->~deleted_type();
	default_allocator::StaticFree(Ptr);
}
