#pragma once

#include <cstdint>
#include <type_traits>
#include "glm/glm.hpp"

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat3 = glm::mat3;
using mat4 = glm::mat4;

using index = u32;
constexpr index InvalidIndex = 0xffffffff;

#define PLATFORM_BREAK() (__debugbreak())
#ifdef NDEBUG
#define FORCEINLINE __forceinline
#define CHECK(condition)
#else
#define FORCEINLINE inline
#define CHECK(condition)  \
	if (!(condition)) {   \
		PLATFORM_BREAK(); \
	}
#endif

#if defined(_MSC_VER) && !defined(__llvm__)
#define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#elif defined(__llvm__)	   // stupid clang warnings
#define NO_UNIQUE_ADDRESS
#else
#define NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

enum class container_clear_type : u8 {
	deallocate,
	dont_deallocate
};

enum class iterator_constness : u8 {
	constant,
	non_constant
};

// stupid templates
template <typename printed_type>
struct compile_print : printed_type::this_field_does_not_exist {};
