#pragma once

#include "Containers/str.h"
#include "core_types.h"
#include "Core/Containers/hash_table.h"

// Number that corresponds to a string (str). Can be created from any str and converted back to str.
// Optimal for copying and comparison, creation from string is usually a big table lookup
struct atom : trait_memcopy_relocatable {
	struct id_pool {
		struct index_hasher {
			[[nodiscard]] FORCEINLINE static hash::hash_type Hash(index Index) {
				return hash::Hash(Pool.mStrings[Index]);
			}
		};

		dyn_array<str> mStrings{};
		hash_set<index, index_hasher> mIndexLookupSet{};
	};

	index mIndex{InvalidIndex};
	static id_pool Pool;

	FORCEINLINE atom() = default;
	FORCEINLINE atom(const atom& OtherId) = default;

	// TODO: kinda arbitrary requirement, supposed to accept str&&, str&, str_view, const char*, maybe should improve
	template <typename string_type>
		requires(!std::is_same_v<atom, string_type> && std::is_constructible_v<string_type, const char*>)
	FORCEINLINE explicit atom(string_type&& Str) {
		if (auto* ExistingIndex = Pool.mIndexLookupSet.FindByPredicate(
				hash::Hash(Str), [&Str](auto& Index) { return Pool.mStrings[Index] == Str; })) {
			mIndex = *ExistingIndex;
		} else {
			mIndex = Pool.mStrings.Add(std::forward(Str));
			Pool.mIndexLookupSet.Add(mIndex);
		}
	}

	[[nodiscard]] FORCEINLINE const str& ToStr() const {
		auto& Strings = Pool.mStrings;
		CHECK(Strings.Size() > mIndex)
		return Strings[mIndex];
	}

	[[nodiscard]] FORCEINLINE bool IsEmpty() const {
		return mIndex == InvalidIndex;
	}

	FORCEINLINE atom& operator=(atom OtherId) {
		mIndex = OtherId.mIndex;
		return *this;
	}

	FORCEINLINE bool operator==(atom OtherId) const {
		return mIndex == OtherId.mIndex;
	}

	FORCEINLINE bool operator!=(atom OtherId) const {
		return mIndex != OtherId.mIndex;
	}

	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::Hash(mIndex);
	}
};