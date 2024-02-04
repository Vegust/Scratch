#pragma once

#include "Containers/dyn_array.h"
#include "Core/Math/math.h"
#include "Templates/equals.h"
#include "Templates/key_value_pair.h"
#include "Core/Hash/hash.h"
#include "Memory/memory.h"

template <typename set_type, iterator_constness Constness>
class hash_set_iter {
public:
	using value_type = set_type::value_type;
	using set_element_type = set_type::set_element_container_type;
	using pointer = std::conditional<Constness == iterator_constness::constant, const value_type*, value_type*>::type;
	using set_element_pointer =
		std::conditional<Constness == iterator_constness::constant, const set_element_type*, set_element_type*>::type;
	using reference = std::conditional<Constness == iterator_constness::constant, const value_type&, value_type&>::type;

private:
	set_element_pointer Element = nullptr;
	set_element_pointer End = nullptr;

public:
	FORCEINLINE hash_set_iter() = default;
	FORCEINLINE hash_set_iter(const hash_set_iter&) = default;
	FORCEINLINE hash_set_iter(hash_set_iter&&) noexcept = default;
	FORCEINLINE ~hash_set_iter() = default;

	FORCEINLINE explicit hash_set_iter(set_element_pointer Element, set_element_pointer End)
		: Element(Element), End(End) {
	}

	FORCEINLINE hash_set_iter& operator++() {
		for (;;) {
			++Element;
			if (Element == End || Element->Hash <= set_type::LastValidHash) {
				break;
			}
		}
		return *this;
	}

	FORCEINLINE pointer operator->() {
		return &(Element->Value);
	}

	FORCEINLINE reference operator*() {
		return (Element->Value);
	}

	FORCEINLINE bool operator==(const hash_set_iter& Other) const {
		return Element == Other.Element;
	}
};

template <
	typename element_type,
	typename hasher = default_hasher,
	typename equals_op = default_equals_op,
	typename allocator_type = default_allocator>
struct hash_set : allocator_instance<allocator_type> {
public:
	constexpr static hash::hash_type EmptyHash = std::numeric_limits<hash::hash_type>::max();
	constexpr static hash::hash_type DeletedHash = std::numeric_limits<hash::hash_type>::max() - 1;
	constexpr static hash::hash_type LastValidHash = std::numeric_limits<hash::hash_type>::max() - 2;
	
	static constexpr bool MemcopyRelocatable = true;

	struct set_elem_container {
		element_type Value;
		hash::hash_type Hash{EmptyHash};
	};

protected:
	set_elem_container* Data = nullptr;
	index Capacity = 0;
	index Size = 0;
	index Deleted = 0;
	index MaxSize = 0;

public:
	using alloc_base = allocator_instance<allocator_type>;
	using iter = hash_set_iter<hash_set, iterator_constness::non_constant>;
	using const_iter = hash_set_iter<hash_set, iterator_constness::constant>;
	using set_element_container_type = set_elem_container;
	using value_type = element_type;
	constexpr static index MinCapacity = 4;
	constexpr static float MaxLoadFactor = 0.7f;
	constexpr static float MaxLoadFactorInverse = 1.f / MaxLoadFactor;
	constexpr static bool FastCopy = trivially_copyable<element_type>;
	constexpr static bool FastDestruct = trivially_destructible<element_type>;
	constexpr static bool MemcopyRealloc = memcopy_relocatable<element_type>;

	struct prober {
		set_elem_container* Data = nullptr;
		index HashMask = 0;
		const hash::hash_type Hash = EmptyHash;
		set_elem_container* SetElem = nullptr;
		index Iteration = 0;

		FORCEINLINE prober(const hash_set& InSet, hash::hash_type InHash)
			: Data(InSet.Data), HashMask(InSet.GetCapacity()), Hash(InHash) {
			HashMask = (1 << math::LogOfTwoCeil(InSet.GetCapacity())) - 1;
			SetElem = Data + (Hash & HashMask);
		}

		FORCEINLINE prober(set_elem_container* InData, index InCapacity, hash::hash_type InHash)
			: Data(InData), Hash(InHash) {
			HashMask = (1 << math::LogOfTwoCeil(InCapacity)) - 1;
			SetElem = Data + (Hash & HashMask);
		}

		[[nodiscard]] FORCEINLINE bool NotEmpty() const {
			return SetElem->Hash != EmptyHash;
		}

		FORCEINLINE prober& operator++() {
			SetElem = Data + ((Hash + TriangleNumber(++Iteration)) & HashMask);
			return *this;
		}

		FORCEINLINE static index TriangleNumber(const index InIteration) {
			return (InIteration * (InIteration + 1)) >> 1;
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
		if (EnsureCapacity(Size + 1)) {
			prober P{*this, GetHash(Key)};
			for (; P.NotEmpty(); ++P) {
				if (P.SetElem->Hash == DeletedHash) {
					--Deleted;
					break;
				}
			}
			++Size;
			P.SetElem->Hash = P.Hash;
			return &(P.SetElem->Value);
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
			if (P.SetElem->Hash == P.Hash && Equals(Elem, P.SetElem->Value)) {
				if constexpr (!FastDestruct) {
					P.SetElem->Value.~element_type();
				}
				P.SetElem->Hash = DeletedHash;
				++Deleted;
				--Size;
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
			if (P.SetElem->Hash == P.Hash && Equals(Elem, P.SetElem->Value)) {
				if constexpr (!FastDestruct) {
					P.SetElem->Value.~element_type();
				}
				P.SetElem->Hash = DeletedHash;
				++Deleted;
				--Size;
				++NumRemoved;
			}
		}
		return NumRemoved;
	}

	FORCEINLINE element_type* Find(const element_type& Elem) const {
		if (!Data) {
			return nullptr;
		}
		for (prober P{*this, GetHash(Elem)}; P.NotEmpty(); ++P) {
			if (P.SetElem->Hash == P.Hash && Equals(Elem, P.SetElem->Value)) {
				return &(P.SetElem->Value);
			}
		}
		return nullptr;
	}

	// When you are calling Find(key) and similar methods, under the hood you probe
	// all matching slots using hash calculated from key and then compare found key with this key.
	// FindByPredicate allows you to provide hash for probing and function to use instead of
	// equality, so you can search hash set without key using arbitrary logic. See atom.h for example
	// use
	template <typename predicate_type>
	FORCEINLINE element_type* FindByPredicate(const hash::hash_type Hash, const predicate_type& Predicate) const {
		if (!Data) {
			return nullptr;
		}
		for (prober P{*this, Hash}; P.NotEmpty(); ++P) {
			if (P.SetElem->Hash == P.Hash && Predicate(P.SetElem->Value)) {
				return &(P.SetElem->Value);
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
		if (!Data) {
			return FoundElements;
		}
		for (prober P{*this, GetHash(Elem)}; P.NotEmpty(); ++P) {
			if (P.SetElem->Hash == P.Hash && Equals(Elem, P.SetElem->Value)) {
				FoundElements.Add(&(P.SetElem->Value));
			}
		}
		return FoundElements;
	}

	FORCEINLINE bool Contains(const element_type& Elem) const {
		return Find(Elem);
	}
	
	[[nodiscard]] FORCEINLINE index GetSize() const {
		return Size;
	}
	
	[[nodiscard]] FORCEINLINE index GetCapacity() const {
		return Capacity;
	}

	FORCEINLINE bool EnsureCapacity(const index NewExpectedSize) {
		if (NewExpectedSize + Deleted > MaxSize) {
			const index DesiredSize =
				NewExpectedSize > MaxSize ? NewExpectedSize : (Size << 1 > MaxSize ? Size << 1 : MaxSize);
			const index DesiredCapacity = 1 << math::LogOfTwoCeil((index) ((DesiredSize + 1) * MaxLoadFactorInverse));
			const index NewCapacity = math::Max(DesiredCapacity, MinCapacity);
			auto* const NewData =
				(set_elem_container*) alloc_base::Allocator.Allocate(NewCapacity * sizeof(set_elem_container));
			if (NewData) {
				for (set_elem_container* SetElem = NewData; SetElem != NewData + NewCapacity; ++SetElem) {
					SetElem->Hash = EmptyHash;
				}
				for (set_elem_container* SetElem = Data; SetElem != Data + Capacity; ++SetElem) {
					if (SetElem->Hash <= LastValidHash) {
						prober P{NewData, NewCapacity, SetElem->Hash};
						for (; P.NotEmpty(); ++P) {
						}
						if constexpr (MemcopyRealloc) {
							std::memcpy(P.SetElem, SetElem, sizeof(set_elem_container));
						} else {
							new (&(P.SetElem->Value)) element_type(std::move(SetElem->Value));
							P.SetElem->Hash = SetElem->Hash;
							SetElem->Value.~value_type();
						}
					}
				}
				alloc_base::Allocator.Free(Data);
				Data = NewData;
				Deleted = 0;
				Capacity = NewCapacity;
				MaxSize = (index) (MaxLoadFactor * Capacity);
				return true;
			}
			return false;
		}
		return true;
	}

	FORCEINLINE void Clear(bool Deallocate = true) {
		if (!FastDestruct) {
			for (set_elem_container* Elem = Data; Elem != Data + Capacity; ++Elem) {
				if (Elem->Hash <= LastValidHash) {
					Elem->Value.~value_type();
				}
			}
		}
		if (Deallocate) {
			if (Data) {
				alloc_base::Allocator.Free(Data);
			}
			Data = nullptr;
			Capacity = 0;
			MaxSize = 0;
		} else {
			for (set_elem_container* Elem = Data; Elem != Data + Capacity; ++Elem) {
				Elem->Hash = EmptyHash;
			}
		}
		Size = 0;
		Deleted = 0;
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
		for (set_elem_container* FirstSetElem = Data; FirstSetElem != Data + Capacity; ++FirstSetElem) {
			if (FirstSetElem->Hash <= LastValidHash) {
				return iter(FirstSetElem, Data + Capacity);
			}
		}
		return iter(Data + Capacity, Data + Capacity);
	}

	FORCEINLINE iter end() {
		return iter(Data + Capacity, Data + Capacity);
	}

	FORCEINLINE const_iter begin() const {
		for (set_elem_container* FirstSetElem = Data; FirstSetElem != Data + Capacity; ++FirstSetElem) {
			if (FirstSetElem->Hash <= LastValidHash) {
				return const_iter(FirstSetElem, Data + Capacity);
			}
		}
		return const_iter(Data + Capacity, Data + Capacity);
	}

	FORCEINLINE const_iter end() const {
		return const_iter(Data + Capacity, Data + Capacity);
	}
};

template <
	typename table_key_type,
	typename table_value_type,
	typename hasher = default_hasher,
	typename equals_op = default_equals_op,
	typename allocator = default_allocator>
class hash_table : public hash_set<key_value_pair<table_key_type, table_value_type>, hasher, equals_op, allocator> {
public:
	using table_pair = key_value_pair<table_key_type, table_value_type>;
	using super = hash_set<table_pair, hasher, equals_op, allocator>;

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
			if (P.SetElem->Hash == P.Hash && super::Equals(Key, P.SetElem->Value)) {
				if constexpr (!super::FastDestruct) {
					P.SetElem->Value.~table_pair();
				}
				P.SetElem->Hash = super::DeletedHash;
				++super::Deleted;
				--super::Size;
				return true;
			}
		}
		return false;
	}

	FORCEINLINE index RemoveAll(const table_key_type& Key) {
		typename super::index_type NumRemoved = 0;
		for (typename super::prober P{*this, super::GetHash(Key)}; P.NotEmpty(); ++P) {
			if (P.SetElem->Hash == P.Hash && super::Equals(Key, P.SetElem->Value)) {
				if constexpr (!super::FastDestruct) {
					P.SetElem->Value.~table_pair();
				}
				P.SetElem->Hash = super::DeletedHash;
				++super::Deleted;
				--super::Size;
				++NumRemoved;
			}
		}
		return NumRemoved;
	}

	FORCEINLINE table_value_type* FindOrAdd(const table_key_type& Key, const table_value_type& DefaultValue) {
		if (table_value_type* FoundElem = Find(Key)) {
			return FoundElem;
		} else {
			return Add(Key, DefaultValue);
		}
	}

	FORCEINLINE dyn_array<table_value_type*> FindAll(const table_key_type& Key) {
		dyn_array<table_value_type*> FoundElements{};
		if (!super::Data) {
			return FoundElements;
		}
		for (typename super::prober P{*this, super::GetHash(Key)}; P.NotEmpty(); ++P) {
			if (P.SetElem->Hash == P.Hash && super::Equals(Key, P.SetElem->Value)) {
				FoundElements.Add(&(P.SetElem->Value.Value));
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
			return Uninitialized->Value;
		}
	}

	FORCEINLINE table_value_type* Find(const table_key_type& Key) const {
		if (table_pair* Pair = FindPair(Key)) {
			return &(Pair->Value);
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
		if (!super::Data) {
			return nullptr;
		}
		for (typename super::prober P{*this, super::GetHash(Key)}; P.NotEmpty(); ++P) {
			if (P.SetElem->Hash == P.Hash && super::Equals(Key, P.SetElem->Value)) {
				return &(P.SetElem->Value);
			}
		}
		return nullptr;
	}
};