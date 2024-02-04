#pragma once

#include "str.h"
#include "str_conversions.h"
#include "atom.h"
#include "format_argument.h"

namespace strings {

index GetFormatLength(str_view String, span<format_argument> Arguments);
index WriteFormat(char* Destination, str_view String, span<format_argument> Arguments);
str Format(str_view String, span<format_argument> Arguments);

template <format_supported... argument_types>
inline str Format(str_view String, const argument_types&... Arguments) {
	array<format_argument, sizeof...(Arguments)> ArgumentArray{Arguments...};
	return Format(String, ArgumentArray);
}

template <format_supported... argument_types>
inline index GetFormatLength(str_view String, const argument_types&... Arguments) {
	array<format_argument, sizeof...(Arguments)> ArgumentArray{Arguments...};
	return GetFormatLength(String, ArgumentArray);
}

template <format_supported... argument_types>
inline index WriteFormat(char* Destination, str_view String, const argument_types&... Arguments) {
	array<format_argument, sizeof...(Arguments)> ArgumentArray{Arguments...};
	return WriteFormat(Destination, String, ArgumentArray);
}

}	 // namespace strings
