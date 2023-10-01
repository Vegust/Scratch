#pragma once

#include "hash.h"

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

	key mKey{};
	value mValue{};

	FORCEINLINE key_value_pair() = default;
	
	FORCEINLINE explicit key_value_pair(const key& Key) : mValue{} {
		mKey = Key;
	}
	
	FORCEINLINE explicit key_value_pair(key&& Key) : mValue{} {
		mKey = std::move(Key);
	}

	FORCEINLINE key_value_pair(key_value_pair&& moved_pair) noexcept
		: mKey(std::move(moved_pair.mKey)), mValue(std::move(moved_pair.mValue)) {
	}

	FORCEINLINE explicit key_value_pair(const key& Key, const value& Value)
		: mKey(Key), mValue(Value) {
	}

	FORCEINLINE ~key_value_pair() = default;

	template <typename other_value>
	FORCEINLINE bool operator==(const key_value_pair<key_type, other_value>& OtherPair) const {
		return mKey == OtherPair.mKey;
	}

	FORCEINLINE bool operator==(const key_type& OtherKey) const {
		return mKey == OtherKey;
	}
	
	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::Hash(mKey);
	}
};