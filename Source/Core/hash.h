#pragma once

#include "core_types.h"

#include <bit>
#include <cstring>

namespace hash {
constexpr u32 DefaultSeed = 0x0131066f;
using hash_type = u32;

[[nodiscard]] constexpr FORCEINLINE hash_type Mix(hash_type Hash) {
	Hash ^= Hash >> 16;
	Hash *= 0x85ebca6b;
	Hash ^= Hash >> 13;
	Hash *= 0xc2b2ae35;
	Hash ^= Hash >> 16;
	return Hash;
}

[[nodiscard]] constexpr FORCEINLINE hash_type MurmurHash(const void* Key, s32 Length) {
	const u8* Data = (const u8*) Key;
	const s32 NumBlocks = Length / 4;
	hash_type Hash = DefaultSeed;
	const hash_type c1 = 0xcc9e2d51;
	const hash_type c2 = 0x1b873593;
	const hash_type* blocks = (const hash_type*) (Data + NumBlocks * 4);
	for (s32 i = -NumBlocks; i; i++) {
		hash_type k1 = blocks[i];
		k1 *= c1;
		k1 = std::rotl(k1, 15);
		k1 *= c2;
		Hash ^= k1;
		Hash = std::rotl(Hash, 13);
		Hash = Hash * 5 + 0xe6546b64;
	}
	const u8* Tail = (const u8*) (Data + NumBlocks * 4);
	hash_type Mask = 0;
	switch (Length & 3) {
		case 3:
			Mask ^= Tail[2] << 16;
		case 2:
			Mask ^= Tail[1] << 8;
		case 1:
			Mask ^= Tail[0];
			Mask *= c1;
			Mask = std::rotl(Mask, 15);
			Mask *= c2;
			Hash ^= Mask;
	};
	Hash ^= Length;
	Hash = Mix(Hash);
	return Hash;
}

[[nodiscard]] FORCEINLINE constexpr hash_type HashCombine(hash_type Hash1, hash_type Hash2) {
	return Hash1 ^ (Hash2 + 0x9e3779b9 + (Hash1 << 6) + (Hash1 >> 2));
}

template <typename type>
	requires requires(const type& Value) { Value.GetHash(); }
[[nodiscard]] FORCEINLINE hash_type Hash(const type& Key) {
	return Key.GetHash();
}

// duck typing support for std string, TODO bad
template <typename string_type>
	requires requires(const string_type& Value) { Value.data(); Value.length(); }
[[nodiscard]] FORCEINLINE hash_type Hash(const string_type& Key) {
	return MurmurHash(Key.data(), Key.length());
}

template <integral integral_type>
[[nodiscard]] FORCEINLINE hash_type Hash(integral_type Key) {
	// return MurmurHash(&mKey, sizeof(T));
	if constexpr (sizeof(integral_type) <= 4) {
		return Mix(Key);
	} else {
		return Mix((hash_type) Key + ((hash_type) (Key >> 32) * 23));
	}
}

template <floating_point floating_type>
[[nodiscard]] FORCEINLINE hash_type Hash(floating_type Key) {
	if constexpr (sizeof(floating_type) <= 4) {
		return Hash(*(hash_type*) &Key);
	} else {
		return Hash(*(hash_type*) &Key);
	}
}

template <enumeration enum_type>
[[nodiscard]] FORCEINLINE hash_type Hash(enum_type Key) {
	return Hash(static_cast<std::underlying_type_t<enum_type>>(Key));
}

[[nodiscard]] FORCEINLINE hash_type Hash(const char* Key) {
	return MurmurHash(Key, (s32) strlen(Key));
}

[[nodiscard]] FORCEINLINE hash_type Hash(const void* Key) {
	const u64 PointerValue = (*(u64*) &Key) >> 4;
	return Hash(PointerValue);
}
}	 // namespace hash

template <typename type>
concept hashable = requires(const type& Value) { hash::Hash(Value); };

struct default_hasher {
	template <hashable hashable_type>
	[[nodiscard]] FORCEINLINE static hash::hash_type Hash(const hashable_type& Key) {
		return hash::Hash(Key);
	}
};