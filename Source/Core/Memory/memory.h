#pragma once

#include "allocator_base.h"
#include "tree_allocator.h"
#include <cstring>

using default_allocator = tree_allocator;

FORCEINLINE void* StaticAlloc(u64 Size, u8 Alignment = 8) {
	return default_allocator::StaticAllocate(Size, Alignment);
}

FORCEINLINE void StaticFree(void* Ptr) {
	default_allocator::StaticFree(Ptr);
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

template <typename swapped_type>
constexpr FORCEINLINE void Memswap(swapped_type& a, swapped_type& b) {
	alignas(swapped_type) u8 tmp_storage[sizeof(swapped_type)] = {0};
	std::memcpy(&tmp_storage, &a, sizeof(swapped_type));
	std::memcpy(&a, &b, sizeof(swapped_type));
	std::memcpy(&b, &tmp_storage, sizeof(swapped_type));
}
