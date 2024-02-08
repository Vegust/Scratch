#pragma once

#include "String/atom.h"
#include "String/str.h"
#include "String/str_format.h"

namespace logs {
namespace atoms {
inline const atom GlobalCategory{"Global Category"};
}
enum class verbosity : u8 { error = 1, warning = 2, info = 3, debug = 4 };
enum class color : u8 { white, yellow, red };
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

constexpr color GetVerbosityColor(verbosity Verbosity) {
	switch (Verbosity) {
		case verbosity::error:
			return color::red;
		case verbosity::warning:
			return color::yellow;
		case verbosity::info:
			return color::white;
		case verbosity::debug:
			return color::white;
	}
}

void Log(
	color Color,
	str_view VerbosityString,
	atom Category,
	str_view FormatString,
	span<strings::format_argument> ArgumentArray);

template <verbosity Verbosity, typename... argument_types>
inline void Log(atom Category, str_view FormatString, const argument_types&... Arguments) {
	if constexpr (static_cast<u8>(EnabledVerbosity) >= static_cast<u8>(Verbosity)) {
		constexpr const color Color = GetVerbosityColor(Verbosity);
		constexpr str_view VerbosityString = GetVerbosityString(Verbosity);
		array<strings::format_argument, sizeof...(Arguments)> ArgumentArray{Arguments...};
		Log(Color, VerbosityString, Category, FormatString, ArgumentArray);
	}
}

template <verbosity Verbosity, typename... argument_types>
inline void Log(str_view FormatString, const argument_types&... Arguments) {
	Log<Verbosity>(atoms::GlobalCategory, FormatString, Arguments...);
}

template <typename... argument_types>
inline void Debug(str_view FormatString, const argument_types&... Arguments) {
	Log<verbosity::debug>(atoms::GlobalCategory, FormatString, Arguments...);
}

template <typename... argument_types>
inline void Info(str_view FormatString, const argument_types&... Arguments) {
	Log<verbosity::info>(atoms::GlobalCategory, FormatString, Arguments...);
}

template <typename... argument_types>
inline void Warning(str_view FormatString, const argument_types&... Arguments) {
	Log<verbosity::warning>(atoms::GlobalCategory, FormatString, Arguments...);
}

template <typename... argument_types>
inline void Error(str_view FormatString, const argument_types&... Arguments) {
	Log<verbosity::error>(atoms::GlobalCategory, FormatString, Arguments...);
}

template <typename... argument_types>
inline void Debug(atom Category, str_view FormatString, const argument_types&... Arguments) {
	Log<verbosity::debug>(Category, FormatString, Arguments...);
}

template <typename... argument_types>
inline void Info(atom Category, str_view FormatString, const argument_types&... Arguments) {
	Log<verbosity::info>(Category, FormatString, Arguments...);
}

template <typename... argument_types>
inline void Warning(atom Category, str_view FormatString, const argument_types&... Arguments) {
	Log<verbosity::warning>(Category, FormatString, Arguments...);
}

template <typename... argument_types>
inline void Error(atom Category, str_view FormatString, const argument_types&... Arguments) {
	Log<verbosity::error>(Category, FormatString, Arguments...);
}

}	 // namespace logs
