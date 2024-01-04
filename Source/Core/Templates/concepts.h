#pragma once

#include "basic.h"
#include "traits.h"

// concepts should if possible be adjectives to distinguish from types

template <typename type>
concept trivially_constructable = std::is_trivially_constructible<type>::value;
template <typename type>
concept trivially_copyable = std::is_trivially_copyable<type>::value;
template <typename type>
concept move_constructable = std::is_move_constructible<type>::value;
template <typename type>
concept copy_constructable = std::is_copy_constructible<type>::value;
template <typename type>
concept trivially_destructible = std::is_trivially_destructible<type>::value;
template <typename type>
concept move_assignable = std::is_move_assignable<type>::value;
template <typename type>
concept copy_assignable = std::is_copy_assignable<type>::value;
template <typename type>
concept integral = std::is_integral<type>::value;
template <typename type>
concept fractional = std::is_floating_point<type>::value;
template <typename type>
concept numeric = integral<type> || fractional<type>;
template<typename lhs_type, typename rhs_type>
concept iso_precise = (integral<lhs_type> && integral<rhs_type>) || (fractional<lhs_type> && fractional<rhs_type>);
template <typename type>
concept pointer = std::is_pointer<type>::value;
template <typename type>
concept enumerable = std::is_enum<type>::value;
template <typename type>
concept memcopy_relocatable = std::is_base_of<trait_memcopy_relocatable, type>::value || trivially_copyable<type>;