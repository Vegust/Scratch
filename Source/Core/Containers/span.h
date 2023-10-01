#pragma once

#include "core_types.h"
#include "array_iter.h"

// non-owning view into str/array/dyn_array
template<typename element_type>
struct span {
	element_type* mData;
	index_type mSize;
	
	using iter = array_iter<span, false>;
	using value_type = element_type;
	using const_iter = array_iter<span, true>;
};