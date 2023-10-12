#pragma once

#include "Containers/dyn_array.h"
#include "Templates/equals.h"
#include "Templates/key_value_pair.h"
#include "core_utility.h"
#include "hash.h"
#include "Memory/memory.h"

template <typename set_type, bool Const>
class hash_set_iter {
public:
	using value_type = set_type::value_type;
	using set_element_type = set_type::set_element_container_type;
	using pointer = std::conditional<Const, const value_type*, value_type*>::type;
	using set_element_pointer =
		std::conditional<Const, const set_element_type*, set_element_type*>::type;
	using reference = std::conditional<Const, const value_type&, value_type&>::type;

private:
	set_element_pointer mElement = nullptr;
	set_element_pointer mEnd = nullptr;

public:
	FORCEINLINE hash_set_iter() = default;
	FORCEINLINE hash_set_iter(const hash_set_iter&) = default;
	FORCEINLINE hash_set_iter(hash_set_iter&&) noexcept = default;
	FORCEINLINE ~hash_set_iter() = default;

	FORCEINLINE explicit hash_set_iter(set_element_pointer Element, set_element_pointer End)
		: mElement(Element), mEnd(End) {
	}

	FORCEINLINE hash_set_iter& operator++() {
		for (;;) {
			++mElement;
			if (mElement == mEnd || mElement->mHash <= set_type::LastValidHash) {
				break;
			}
		}
		return *this;
	}

	FORCEINLINE pointer operator->() {
		return &(mElement->mValue);
	}

	FORCEINLINE reference operator*() {
		return (mElement->mValue);
	}

	FORCEINLINE bool operator==(const hash_set_iter& Other) const {
		return mElement == Other.mElement;
	}
};

template <
	typename element_type,
	typename hasher = default_hasher,
	typename equals_op = default_equals_op,
	typename allocator_type = default_allocator>
class hash_set : allocator_instance<allocator_type>, trait_memcopy_relocatable {
public:
	constexpr static hash::hash_type EmptyHash = std::numeric_limits<hash::hash_type>::max();
	constexpr static hash::hash_type DeletedHash = std::numeric_limits<hash::hash_type>::max() - 1;
	constexpr static hash::hash_type LastValidHash =
		std::numeric_limits<hash::hash_type>::max() - 2;

	struct set_elem_container {
		element_type mValue;
		hash::hash_type mHash{EmptyHash};
	};

	set_elem_container* mData = nullptr;
	index mCapacity = 0;
	index mSize = 0;
	index mDeleted = 0;
	index mMaxSize = 0;

	using alloc_base = allocator_instance<allocator_type>;
	using iter = hash_set_iter<hash_set, false>;
	using const_iter = hash_set_iter<hash_set, true>;
	using set_element_container_type = set_elem_container;
	using value_type = element_type;
	constexpr static index MinCapacity = 4;
	constexpr static float MaxLoadFactor = 0.7f;
	constexpr static float MaxLoadFactorInverse = 1.f / MaxLoadFactor;
	constexpr static bool FastCopy = trivially_copyable<element_type>;
	constexpr static bool FastDestruct = trivially_destructible<element_type>;
	constexpr static bool MemcopyRealloc = memcopy_relocatable<element_type>;

	constexpr static bool DebugMode = false;
	static inline int NumCollisions = 0;
	static inline int NumQueries = 0;
	static inline int CurrentProbe = 0;
	static inline int LongestProbe = 0;
	static inline int ProbeHistogram[64]{0};

	static void PrintDebugInfo() {
		//		std::cout << "Collision/Query = " << (float) NumCollisions / NumQueries
		//				  << ". Longest probe = " << LongestProbe << std::endl;
		//		for (int i = 0; i < 64; ++i) {
		//			std::cout << i << " collisions: " << ProbeHistogram[i] << " ("
		//					  << 100 * ((float) ProbeHistogram[i] / NumQueries) << "%)" <<
		// std::endl;
		//		}
	}

	struct prober {
		set_elem_container* mData = nullptr;
		index mHashMask = 0;
		const hash::hash_type mHash = EmptyHash;
		set_elem_container* mSetElem = nullptr;
		index mIteration = 0;

		FORCEINLINE prober(const hash_set& Set, hash::hash_type Hash)
			: mData(Set.mData), mHashMask(Set.mCapacity), mHash(Hash) {
			if constexpr (DebugMode) {
				++NumQueries;
				if (CurrentProbe < 64) {
					ProbeHistogram[CurrentProbe] += 1;
				}
				LongestProbe = Max(LongestProbe, CurrentProbe);
				CurrentProbe = 0;
			}
			mHashMask = (1 << LogOfTwoCeil(Set.mCapacity)) - 1;
			mSetElem = mData + (mHash & mHashMask);
		}

		FORCEINLINE prober(set_elem_container* Data, index Capacity, hash::hash_type Hash)
			: mData(Data), mHash(Hash) {
			if constexpr (DebugMode) {
				++NumQueries;
				if (CurrentProbe < 64) {
					ProbeHistogram[CurrentProbe] += 1;
				}
				LongestProbe = Max(LongestProbe, CurrentProbe);
				CurrentProbe = 0;
			}
			mHashMask = (1 << LogOfTwoCeil(Capacity)) - 1;
			mSetElem = mData + (mHash & mHashMask);
		}

		[[nodiscard]] FORCEINLINE bool NotEmpty() const {
			if constexpr (DebugMode) {
				const bool Collision = mSetElem->mHash != EmptyHash;
				if (Collision) {
					++NumCollisions;
					++CurrentProbe;
				}
				return Collision;
			}
			return mSetElem->mHash != EmptyHash;
		}

		FORCEINLINE prober& operator++() {
			mSetElem = mData + ((mHash + TriangleNumber(++mIteration)) & mHashMask);
			return *this;
		}

		FORCEINLINE index TriangleNumber(const index Iteration) {
			return (Iteration * (Iteration + 1)) >> 1;
		}
	};

	FORCEINLINE hash_set() = default;

	FORCEINLINE explicit hash_set(index InitialSize) {
		EnsureCapacity(InitialSize);
	}

	FORCEINLINE ~hash_set() {
		Clear();
	}

	FORCEINLINE element_type* Add(const element_type& Elem) {
		if (void* Position = AddUninitialized(Elem)) {
			return new (Position) element_type(Elem);
		}
		return nullptr;
	}

	template <typename key_type>
	FORCEINLINE void* AddUninitialized(const key_type& Key) {
		if (EnsureCapacity(mSize + 1)) {
			prober P{*this, GetHash(Key)};
			for (; P.NotEmpty(); ++P) {
				if (P.mSetElem->mHash == DeletedHash) {
					--mDeleted;
					break;
				}
			}
			++mSize;
			P.mSetElem->mHash = P.mHash;
			return &(P.mSetElem->mValue);
		}
		return nullptr;
	}

	FORCEINLINE element_type* AddUnique(const element_type& Elem) {
		if (element_type* FoundValue = Find(Elem)) {
			if constexpr (!FastDestruct) {
				FoundValue->~element_type();
			}
			return new (FoundValue) element_type(Elem);
		}
		return Add(Elem);
	}

	FORCEINLINE bool RemoveOne(const element_type& Elem) {
		for (prober P{*this, GetHash(Elem)}; P.NotEmpty(); ++P) {
			if (P.mSetElem->mHash == P.mHash && Equals(Elem, P.mSetElem->mValue)) {
				if constexpr (!FastDestruct) {
					P.mSetElem->mValue.~element_type();
				}
				P.mSetElem->mHash = DeletedHash;
				++mDeleted;
				--mSize;
				return true;
			}
		}
		return false;
	}

	FORCEINLINE bool Remove(const element_type& Elem) {
		return RemoveOne(Elem);
	}

	FORCEINLINE index RemoveAll(const element_type& Elem) {
		index NumRemoved = 0;
		for (prober P{*this, GetHash(Elem)}; P.NotEmpty(); ++P) {
			if (P.mSetElem->mHash == P.mHash && Equals(Elem, P.mSetElem->mValue)) {
				if constexpr (!FastDestruct) {
					P.mSetElem->mValue.~element_type();
				}
				P.mSetElem->mHash = DeletedHash;
				++mDeleted;
				--mSize;
				++NumRemoved;
			}
		}
		return NumRemoved;
	}

	FORCEINLINE element_type* Find(const element_type& Elem) const {
		if (!mData) {
			return nullptr;
		}
		for (prober P{*this, GetHash(Elem)}; P.NotEmpty(); ++P) {
			if (P.mSetElem->mHash == P.mHash && Equals(Elem, P.mSetElem->mValue)) {
				return &(P.mSetElem->mValue);
			}
		}
		return nullptr;
	}

	// When you are calling Find(key) and similar methods, under the hood you probe
	// all matching slots using hash calculated from key and then compare found key with this key.
	// FindByPredicate allows you to provide hash for probing and function to use instead of
	// equality, so you can search hash set without key using arbitrary logic. See tag.h for example
	// use
	template <typename predicate_type>
	FORCEINLINE element_type* FindByPredicate(const hash::hash_type Hash, const predicate_type& Predicate)
		const {
		if (!mData) {
			return nullptr;
		}
		for (prober P{*this, Hash}; P.NotEmpty(); ++P) {
			if (P.mSetElem->mHash == P.mHash && Predicate(P.mSetElem->mValue)) {
				return &(P.mSetElem->mValue);
			}
		}
		return nullptr;
	}

	FORCEINLINE element_type* FindOrAdd(const element_type& Elem) {
		if (element_type* FoundElem = Find(Elem)) {
			return FoundElem;
		} else {
			return Add(Elem);
		}
	}

	FORCEINLINE dyn_array<element_type*> FindAll(const element_type& Elem) {
		dyn_array<element_type*> FoundElements{};
		if (!mData) {
			return FoundElements;
		}
		for (prober P{*this, GetHash(Elem)}; P.NotEmpty(); ++P) {
			if (P.mSetElem->mHash == P.mHash && Equals(Elem, P.mSetElem->mValue)) {
				FoundElements.Add(&(P.mSetElem->mValue));
			}
		}
		return FoundElements;
	}

	FORCEINLINE bool Contains(const element_type& Elem) const {
		return Find(Elem);
	}

	// Unlike array, hash_set does not have maximum allocation size that it cannot exceed when
	// doubling size, because hash_set relies on capacity being a power of two (masking, triangle
	// numbers for probing)
	FORCEINLINE bool EnsureCapacity(const index NewExpectedSize) {
		if (NewExpectedSize + mDeleted > mMaxSize) {
			const index DesiredSize = NewExpectedSize > mMaxSize
											   ? NewExpectedSize
											   : (mSize << 1 > mMaxSize ? mSize << 1 : mMaxSize);
			const index DesiredCapacity =
				1 << LogOfTwoCeil((index) ((DesiredSize + 1) * MaxLoadFactorInverse));
			const index NewCapacity = Max(DesiredCapacity, MinCapacity);
			auto* const NewData =
				(set_elem_container*) alloc_base::mAllocator.Allocate(NewCapacity * sizeof(set_elem_container));
			if (NewData) {
				for (set_elem_container* SetElem = NewData; SetElem != NewData + NewCapacity;
					 ++SetElem) {
					SetElem->mHash = EmptyHash;
				}
				for (set_elem_container* SetElem = mData; SetElem != mData + mCapacity; ++SetElem) {
					if (SetElem->mHash <= LastValidHash) {
						prober P{NewData, NewCapacity, SetElem->mHash};
						for (; P.NotEmpty(); ++P) {
						}
						if constexpr (MemcopyRealloc) {
							std::memcpy(P.mSetElem, SetElem, sizeof(set_elem_container));
						} else {
							new (&(P.mSetElem->mValue)) element_type(std::move(SetElem->mValue));
							P.mSetElem->mHash = SetElem->mHash;
							SetElem->mValue.~value_type();
						}
					}
				}
				alloc_base::mAllocator.Free(mData);
				mData = NewData;
				mDeleted = 0;
				mCapacity = NewCapacity;
				mMaxSize = (index) (MaxLoadFactor * mCapacity);
				return true;
			}
			return false;
		}
		return true;
	}

	FORCEINLINE void Clear(bool Deallocate = true) {
		if (!FastDestruct) {
			for (set_elem_container* Elem = mData; Elem != mData + mCapacity; ++Elem) {
				if (Elem->mHash <= LastValidHash) {
					Elem->mValue.~value_type();
				}
			}
		}
		if (Deallocate) {
			if (mData) {
				alloc_base::mAllocator.Free(mData);
			}
			mData = nullptr;
			mCapacity = 0;
			mMaxSize = 0;
		} else {
			for (set_elem_container* Elem = mData; Elem != mData + mCapacity; ++Elem) {
				Elem->mHash = EmptyHash;
			}
		}
		mSize = 0;
		mDeleted = 0;
	}

	template <typename HashedType>
	FORCEINLINE hash::hash_type GetHash(const HashedType& Elem) const {
		const hash::hash_type Hash = hasher::Hash(Elem);
		return Hash - (Hash > LastValidHash) * 2;
	}

	template <typename LhsType, typename RhsType>
	FORCEINLINE bool Equals(const LhsType& Lhs, const RhsType& Rhs) const {
		return equals_op::Equals(Lhs, Rhs);
	}

	FORCEINLINE iter begin() {
		for (set_elem_container* FirstSetElem = mData; FirstSetElem != mData + mCapacity;
			 ++FirstSetElem) {
			if (FirstSetElem->mHash <= LastValidHash) {
				return iter(FirstSetElem, mData + mCapacity);
			}
		}
		return iter(mData + mCapacity, mData + mCapacity);
	}

	FORCEINLINE iter end() {
		return iter(mData + mCapacity, mData + mCapacity);
	}

	FORCEINLINE const_iter begin() const {
		for (set_elem_container* FirstSetElem = mData; FirstSetElem != mData + mCapacity;
			 ++FirstSetElem) {
			if (FirstSetElem->mHash <= LastValidHash) {
				return const_iter(FirstSetElem, mData + mCapacity);
			}
		}
		return const_iter(mData + mCapacity, mData + mCapacity);
	}

	FORCEINLINE const_iter end() const {
		return const_iter(mData + mCapacity, mData + mCapacity);
	}
};

template <
	typename table_key_type,
	typename table_value_type,
	typename hasher = default_hasher,
	typename equals_op = default_equals_op,
	typename allocator = default_allocator>
class hash_table
	: public hash_set<key_value_pair<table_key_type, table_value_type>, hasher, equals_op, allocator> {
public:
	using table_pair = key_value_pair<table_key_type, table_value_type>;
	using super = hash_set<table_pair, hasher, equals_op, allocator>;
	using prober = typename super::prober;

	FORCEINLINE explicit hash_table() = default;

	FORCEINLINE explicit hash_table(index InitialSize) {
		super::EnsureCapacity(InitialSize);
	}

	FORCEINLINE table_pair* Add(const table_key_type& Key, const table_value_type& Value) {
		if (void* Position = super::AddUninitialized(Key)) {
			return new (Position) table_pair(Key, Value);
		}
		return nullptr;
	}

	FORCEINLINE table_pair* AddUnique(const table_key_type& Key, const table_value_type& Value) {
		if (table_pair* FoundValue = FindPair(Key)) {
			if constexpr (!super::FastDestruct) {
				FoundValue->~table_pair();
			}
			return new (FoundValue) table_pair(Key, Value);
		}
		return Add(Key, Value);
	}

	FORCEINLINE bool Remove(const table_key_type& Key) {
		return RemoveOne(Key);
	}

	FORCEINLINE bool RemoveOne(const table_key_type& Key) {
		for (typename super::prober P{*this, super::GetHash(Key)}; P.NotEmpty(); ++P) {
			if (P.mSetElem->mHash == P.mHash && super::Equals(Key, P.mSetElem->mValue)) {
				if constexpr (!super::FastDestruct) {
					P.mSetElem->mValue.~table_pair();
				}
				P.mSetElem->mHash = super::DeletedHash;
				++super::mDeleted;
				--super::mSize;
				return true;
			}
		}
		return false;
	}

	FORCEINLINE index RemoveAll(const table_key_type& Key) {
		typename super::index_type NumRemoved = 0;
		for (typename super::prober P{*this, super::GetHash(Key)}; P.NotEmpty(); ++P) {
			if (P.mSetElem->mHash == P.mHash && super::Equals(Key, P.mSetElem->mValue)) {
				if constexpr (!super::FastDestruct) {
					P.mSetElem->mValue.~table_pair();
				}
				P.mSetElem->mHash = super::DeletedHash;
				++super::mDeleted;
				--super::mSize;
				++NumRemoved;
			}
		}
		return NumRemoved;
	}

	FORCEINLINE table_value_type* FindOrAdd(
		const table_key_type& Key,
		const table_value_type& DefaultValue) {
		if (table_value_type* FoundElem = Find(Key)) {
			return FoundElem;
		} else {
			return Add(Key, DefaultValue);
		}
	}

	FORCEINLINE dyn_array<table_value_type*> FindAll(const table_key_type& Key) {
		dyn_array<table_value_type*> FoundElements{};
		if (!super::mData) {
			return FoundElements;
		}
		for (typename super::prober P{*this, super::GetHash(Key)}; P.NotEmpty(); ++P) {
			if (P.mSetElem->mHash == P.mHash && super::Equals(Key, P.mSetElem->mValue)) {
				FoundElements.Add(&(P.mSetElem->mValue.mValue));
			}
		}
		return FoundElements;
	}

	[[nodiscard]] FORCEINLINE table_value_type& operator[](const table_key_type& Key) {
		if (table_value_type* FoundElem = Find(Key)) {
			return *FoundElem;
		} else {
			auto* Uninitialized = (table_pair*) (super::AddUninitialized(Key));
			new (Uninitialized) table_pair{Key};
			return Uninitialized->mValue;
		}
	}

	FORCEINLINE table_value_type* Find(const table_key_type& Key) const {
		if (table_pair* Pair = FindPair(Key)) {
			return &(Pair->mValue);
		}
		return nullptr;
	}

	FORCEINLINE bool Contains(const table_key_type& Key) const {
		return Find(Key);
	}

	FORCEINLINE bool ContainsKey(const table_key_type& Key) const {
		return Find(Key);
	}

	FORCEINLINE table_pair* FindPair(const table_key_type& Key) const {
		if (!super::mData) {
			return nullptr;
		}
		for (typename super::prober P{*this, super::GetHash(Key)}; P.NotEmpty(); ++P) {
			if (P.mSetElem->mHash == P.mHash && super::Equals(Key, P.mSetElem->mValue)) {
				return &(P.mSetElem->mValue);
			}
		}
		return nullptr;
	}
};