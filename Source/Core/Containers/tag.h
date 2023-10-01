#pragma once

#include "Containers/str.h"
#include "core_types.h"
#include "hash_table.h"

// Number that corresponds to a string (str). Can be created from any str and converted back to str.
// Optimal for copying and comparison. Similar to Unreal FName, but without instance numbers
struct tag : trait_memcopy_relocatable {
	struct id_pool {
		struct index_hasher {
			[[nodiscard]] FORCEINLINE static hash::hash_type Hash(index_type Index) {
				return hash::Hash(Pool.mStrings[Index]);
			}
		};
		
		// NOTE: this should not allocate memory at construction
		// because static allocator might not be initialized
		dyn_array<str> mStrings{};
		hash_set<index_type, index_hasher> mIndexLookupSet{};
	};

	index_type mIndex{InvalidIndex};
	static id_pool Pool;

	FORCEINLINE tag() = default;
	FORCEINLINE tag(const tag& OtherId) = default;

	FORCEINLINE explicit tag(const str& Str) {
		if (auto* ExistingIndex = FindExistingIndex(Str.GetHash(), Str)) {
			mIndex = *ExistingIndex;
		} else {
			mIndex = Pool.mStrings.Add(Str);
			Pool.mIndexLookupSet.Add(mIndex);
		}
	}

	FORCEINLINE explicit tag(str&& Str) {
		if (auto* ExistingIndex = FindExistingIndex(Str.GetHash(), Str)) {
			mIndex = *ExistingIndex;
		} else {
			mIndex = Pool.mStrings.Emplace(std::move(Str));
			Pool.mIndexLookupSet.Add(mIndex);
		}
	}

	FORCEINLINE explicit tag(const char* Chars) {
		if (auto* ExistingIndex = FindExistingIndex(hash::Hash(Chars), Chars)) {
			mIndex = *ExistingIndex;
		} else {
			mIndex = Pool.mStrings.Emplace(Chars);
			Pool.mIndexLookupSet.Add(mIndex);
		}
	}

	template <typename string_type>
	FORCEINLINE index_type* FindExistingIndex(hash::hash_type Hash, string_type&& String) {
		return Pool.mIndexLookupSet.FindByPredicate(
			Hash, [&String](auto& Index) { return Pool.mStrings[Index] == String; });
	}

	[[nodiscard]] FORCEINLINE const str& ToStr() const {
		auto& Strings = Pool.mStrings;
		CHECK(Strings.Size() > mIndex)
		return Strings[mIndex];
	}

	[[nodiscard]] FORCEINLINE bool IsEmpty() const {
		return mIndex == InvalidIndex;
	}

	FORCEINLINE tag& operator=(tag OtherId) {
		mIndex = OtherId.mIndex;
		return *this;
	}

	FORCEINLINE bool operator==(tag OtherId) const {
		return mIndex == OtherId.mIndex;
	}

	FORCEINLINE bool operator!=(tag OtherId) const {
		return mIndex != OtherId.mIndex;
	}

	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::Hash(mIndex);
	}
};