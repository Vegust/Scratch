#pragma once

#include "basic.h"
#include "String/str.h"
#include "Containers/array.h"
#include "Containers/hash_table.h"
#include "Hash/hash.h"

// Number that corresponds to a string (str). Can be created from any str and converted back to str.
// Optimal for copying and comparison, creation from string is a big table lookup
struct atom {
	struct atom_pool {
		struct index_hasher {
			static hash::hash_type Hash(index Index);
		};

		// NOTE: This is redundant but Clang wants it really bad
		atom_pool() : Strings{}, IndexLookupSet{} {
		}

		dyn_array<str> Strings{};
		hash_set<index, index_hasher> IndexLookupSet{};
	};

	static constexpr bool MemcopyRelocatable = true;

	static atom_pool& GetAtomPool();

	index Index{InvalidIndex};

	FORCEINLINE atom() = default;
	FORCEINLINE atom(const atom& OtherId) = default;
	FORCEINLINE atom& operator=(const atom& OtherId) = default;

	FORCEINLINE explicit atom(str_view Str) {
		auto& Pool = GetAtomPool();
		if (auto* ExistingIndex = Pool.IndexLookupSet.FindByPredicate(
				hash::Hash(Str), [&Pool, Str](auto& Index) { return Pool.Strings[Index] == Str; })) {
			Index = *ExistingIndex;
		} else {
			Index = Pool.Strings.Emplace(Str);
			Pool.IndexLookupSet.Add(Index);
		}
	}

	[[nodiscard]] FORCEINLINE const str& ToStr() const {
		auto& Strings = GetAtomPool().Strings;
		CHECK(Strings.GetSize() > Index)
		return Strings[Index];
	}

	[[nodiscard]] FORCEINLINE bool IsEmpty() const {
		return Index == InvalidIndex;
	}

	FORCEINLINE bool operator==(atom OtherId) const {
		return Index == OtherId.Index;
	}

	FORCEINLINE bool operator!=(atom OtherId) const {
		return Index != OtherId.Index;
	}

	[[nodiscard]] FORCEINLINE hash::hash_type GetHash() const {
		return hash::Hash(Index);
	}
};