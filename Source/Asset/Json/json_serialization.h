#pragma once

#include "basic.h"
#include "String/str.h"
#include "Reflection/reflection.h"

namespace json {



template <reflection::supported serializable_type>
result<serializable_type> Read(str_view Path) {
}

template <reflection::supported serializable_type>
void Write(const serializable_type& Value, str_view Path) {
	__m512 t;
}
}	 // namespace json