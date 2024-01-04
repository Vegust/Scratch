#pragma once

#include "Core/Utility/hash.h"
#include "Core/Templates/concepts.h"

// TODO bad
template <typename key, typename value>
struct conditionally_memcopy_relocatable {};

template <typename key, typename value>
	requires(memcopy_relocatable<key> && memcopy_relocatable<value>)
struct conditionally_memcopy_relocatable<key, value> : trait_memcopy_relocatable {};

// Pair of key and value, where, by default, hashing and less/equals operators are
// only performed using key, and value is ignored
template <typename key, typename value>
struct key_value_pair : conditionally_memcopy_relocatable<key, value> {
	using key_type = key;
	using value_type = value;

	key Key{};
	value Value{};

	FORCEINLINE key_value_pair() = default;
	
	FORCEINLINE explicit key_value_pair(const key& InKey) : Value{} {
		Key = InKey;
	}
	
	FORCEINLINE explicit key_value_pair(key&& InKey) : Value{} {
		Key = std::move(InKey);
	}

	FORCEINLINE key_value_pair(key_value_pair&& moved_pair) noexcept
		: Key(std::move(moved_pair.Key)), Value(std::move(moved_pair.Value)) {
	}

	FORCEINLINE explicit key_value_pair(const key& InKey, const value& InValue)
		: Key(InKey), Value(InValue) {
	}

	FORCEINLINE ~key_value_pair() = default;

	template <typename other_value>
	FORCEINLINE bool operator==(const key_value_pair<key_type, other_value>& OtherPair) const {
		return Key == OtherPair.Key;
	}

	FORCEINLINE bool operator==(const key_type& OtherKey) const {
		return Key == OtherKey;
	}
	
	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::Hash(Key);
	}
};