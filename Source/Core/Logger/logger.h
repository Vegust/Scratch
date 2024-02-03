#pragma once

#include "String/atom.h"
#include "String/str.h"
#include "String/str_format.h"

namespace logs {
enum class verbosity : u8 { error = 1, warning = 2, info = 3, debug = 4 };
constexpr verbosity EnabledVerbosity = verbosity::debug;

constexpr str_view GetVerbosityString(verbosity Verbosity) {
	switch (Verbosity) {
		case verbosity::error:
			return "ERROR  ";
		case verbosity::warning:
			return "WARNING";
		case verbosity::info:
			return "INFO   ";
		case verbosity::debug:
			return "DEBUG  ";
	}
}

template <verbosity Verbosity = verbosity::info, typename... argument_types>
inline void Log(atom Category, str_view String, const argument_types&... Arguments) {
	if constexpr (static_cast<u8>(EnabledVerbosity) >= static_cast<u8>(Verbosity)) {
		const index CategoryLength = Category.ToStr().GetByteLength();
		const strings::format_argument_array<sizeof...(Arguments)> ArgumentArray{Arguments...};
		const index InputLength = strings::GetFormatLength(String, ArgumentArray);
		constexpr str_view VerbosityString = GetVerbosityString(Verbosity);
		const str_view TimeString{"03.02.2024 16:30:59.1005"};	  // TODO real time
		const index TotalLength =
			TimeString.GetSize() + 3 + VerbosityString.GetSize() + 3 + CategoryLength + 3 + InputLength;
		str LogString;
		LogString.Reserve(TotalLength + 1);
		LogString += TimeString;
		LogString += " | ";
		LogString += VerbosityString;
		LogString += " | ";
		LogString += Category.ToStr();
		LogString += " | ";
		strings::WriteFormat(LogString.GetData() + LogString.GetByteLength(), String, ArgumentArray);
		LogString.OverwriteSize(TotalLength + 1);
		LogString[TotalLength] = 0;
		if constexpr (Verbosity == verbosity::error) {
			std::cout << "\033[1;31m";
		}
		if constexpr (Verbosity == verbosity::warning) {
			std::cout << "\033[1;33m";
		}
		std::cout << LogString << "\n";	   // TODO real output
		if constexpr (Verbosity == verbosity::warning || Verbosity == verbosity::error) {
			std::cout << "\033[1;0m";
		}
	}
}

template <verbosity Verbosity = verbosity::info, typename... argument_types>
inline void Log(str_view String, const argument_types&... Arguments) {
	if constexpr (static_cast<u8>(EnabledVerbosity) >= static_cast<u8>(Verbosity)) {
		static atom Global{"Global"};
		Log<Verbosity>(Global, String, Arguments...);
	}
}

}	 // namespace logs
