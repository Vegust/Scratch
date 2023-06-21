//
// Created by Vegust on 17.06.2023.
//

#pragma once

#include <cstdint>
#include <type_traits>

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

#define SCRATCH_DISABLE_WARNINGS_BEGIN()                                       \
	_Pragma("clang diagnostic push");                                          \
	_Pragma("clang diagnostic ignored \"-Wall\"");                             \
	_Pragma("clang diagnostic ignored \"-Wextra\"");                           \
	_Pragma("clang diagnostic ignored \"-Wpedantic\"");                        \
	_Pragma("clang diagnostic ignored \"-Wnonportable-system-include-path\""); \
	_Pragma("clang diagnostic ignored \"-Wdocumentation-unknown-command\"");   \
	_Pragma("clang diagnostic ignored \"-Wreserved-macro-identifier\"");       \
	_Pragma("clang diagnostic ignored \"-Wreserved-identifier\"");             \
	_Pragma("clang diagnostic ignored \"-Wcast-function-type-strict\"");       \
	_Pragma("clang diagnostic ignored \"-Wnewline-eof\"");                        \
    _Pragma("clang diagnostic ignored \"-Wunsafe-buffer-usage\"");      \
	_Pragma("clang diagnostic ignored \"-Wdocumentation\"");

#define SCRATCH_DISABLE_WARNINGS_END() _Pragma("clang diagnostic pop");
