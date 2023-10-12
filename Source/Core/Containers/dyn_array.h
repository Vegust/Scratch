#pragma once

#include "Templates/less.h"
#include "Memory/memory.h"
#include "core_utility.h"
#include "array_iter.h"

#include <initializer_list>
#include <limits>


template <typename element_type, u64 StackSize>
struct array_stack_storage {
public:
	array_stack_storage() {
	}

protected:
	NO_UNIQUE_ADDRESS alignas(element_type) u8 mStackStorage[StackSize * sizeof(element_type)] = {
		0};
};

template <typename element_type>
class array_stack_storage<element_type, 0> {};

template <typename element_type, typename allocator_type = default_allocator, index StackSize = 0>
struct dyn_array : array_stack_storage<element_type, StackSize>,
			   trait_memcopy_relocatable,
			   allocator_instance<allocator_type> {
public:
	element_type* mData = nullptr;
	index mSize = 0;
	index mCapacity = StackSize;

	using iter = array_iter<dyn_array, false>;
	using value_type = element_type;
	using const_iter = array_iter<dyn_array, true>;
	using alloc_base = allocator_instance<allocator_type>;
	constexpr static bool FastCopy = trivially_copyable<element_type>;
	constexpr static bool FastDestruct = trivially_destructible<element_type>;
	constexpr static bool MemcopyRealloc = memcopy_relocatable<element_type>;
	constexpr static index MaxSingleByteSizeIncrease = 1024 * 1024 * 10;	 // 10 MB,
																				 // completely
																				 // arbitrary
	constexpr static index MaxSingleCapacityIncrease =
		Max(1, MaxSingleByteSizeIncrease / sizeof(element_type));

	dyn_array() = default;

	FORCEINLINE ~dyn_array() {
		Clear();
	}

	FORCEINLINE explicit dyn_array(index InitialCapacity) {
		EnsureCapacity(InitialCapacity);
	}

	FORCEINLINE explicit dyn_array(element_type* Source, index Count) {
		mData = Source;
		mCapacity = Count;
		mSize = Count;
	}

	FORCEINLINE explicit dyn_array(const element_type* Source, index Count) {
		EnsureCapacity(Count);
		CopyConstructElements(Data(), Source, Count);
		mSize = Count;
	}

	FORCEINLINE dyn_array(std::initializer_list<element_type> InitList)
		: dyn_array(InitList.begin(), InitList.size()) {
	}

	FORCEINLINE dyn_array(const dyn_array& Other) {
		EnsureCapacity(Other.mSize);
		CopyConstructElements(Data(), Other.Data(), Other.mSize);
		mSize = Other.mSize;
	}

	FORCEINLINE dyn_array(dyn_array&& Other) noexcept {
		GrabFromOther(std::move(Other));
	}

	FORCEINLINE bool operator==(const dyn_array& Other) const {
		if (mSize != Other.mSize) {
			return false;
		}
		for (index i = 0; i < mSize; ++i) {
			if (Other[i] != operator[](i)) {
				return false;
			}
		}
		return true;
	}

	FORCEINLINE dyn_array& AppendInplace(const dyn_array& Other) {
		EnsureCapacity(mSize + Other.mSize);
		CopyConstructElements(end(), Other.Data(), Other.mSize);
		mSize = mSize + Other.mSize;
		return *this;
	}

	FORCEINLINE dyn_array& AppendInplace(dyn_array&& Other) {
		EnsureCapacity(mSize + Other.mSize);
		std::memcpy(end(), Other.Data(), Other.mSize * sizeof(element_type));
		mSize = mSize + Other.mSize;
		return *this;
	}

	FORCEINLINE dyn_array& operator=(std::initializer_list<element_type> InitList) {
		Clear(false);
		EnsureCapacity(InitList.size());
		CopyConstructElements(Data(), InitList.begin(), InitList.size());
		mSize = InitList.size();
		return *this;
	}

	FORCEINLINE dyn_array& operator=(dyn_array&& Other) noexcept {
		if (&Other == this) {
			return *this;
		}
		Clear(false);
		GrabFromOther(std::move(Other));

		return *this;
	}

	FORCEINLINE dyn_array& operator=(const dyn_array& Other) {
		if (&Other == this) {
			return *this;
		}

		Clear(false);
		EnsureCapacity(Other.mSize);
		CopyConstructElements(Data(), Other.Data(), Other.mSize);
		mSize = Other.mSize;

		return *this;
	}

	FORCEINLINE element_type* Data() {
		if constexpr (StackSize > 0) {
			if (mCapacity <= StackSize) {
				return reinterpret_cast<element_type*>(this->mStackStorage);
			}
		}
		return mData;
	}

	FORCEINLINE const element_type* Data() const {
		if constexpr (StackSize > 0) {
			if (mCapacity <= StackSize) {
				return reinterpret_cast<const element_type*>(this->mStackStorage);
			}
		}
		return mData;
	}

	FORCEINLINE index Capacity() const {
		return mCapacity;
	}

	FORCEINLINE index Size() const {
		return mSize;
	}

	FORCEINLINE element_type& operator[](const index Index) {
		return Data()[Index];
	}

	FORCEINLINE const element_type& operator[](const index Index) const {
		return Data()[Index];
	}

	template <typename... TArgs>
	FORCEINLINE index Emplace(TArgs&&... Args) {
		return EmplaceAt(mSize, std::forward<TArgs>(Args)...);
	}

	FORCEINLINE index Add(const element_type& Element) {
		return AddAt(mSize, Element);
	}

	FORCEINLINE index AddAt(const index Index, const element_type& Element) {
		if (EnsureCapacity(mSize + 1)) {
			ShiftForwardOne(Index);	   // TODO: potentially 1 redundant memmove
			new (&(Data()[Index])) element_type(Element);
			++mSize;
			return Index;
		}
		return InvalidIndex;
	}

	template <typename... TArgs>
	FORCEINLINE index EmplaceAt(const index Index, TArgs&&... Args) {
		if (EnsureCapacity(mSize + 1)) {
			ShiftForwardOne(Index);	   // TODO: potentially 1 redundant move
			new (&(Data()[Index])) element_type(std::forward<TArgs>(Args)...);
			++mSize;
			return Index;
		}
		return InvalidIndex;
	}

	FORCEINLINE void RemoveAt(const index Index) {
		DestroyElement(Index);
		ShiftBackOne(Index + 1);
		--mSize;
	}

	FORCEINLINE void RemoveAtSwap(const index Index) {
		if constexpr (StackSize > 0) {
			element_type* ActualData = Data();
			if constexpr (MemcopyRealloc) {
				DestroyElement(Index);
				std::memmove(&ActualData[Index], &ActualData[mSize - 1], sizeof(element_type));
			} else {
				SwapElements(ActualData[Index], ActualData[mSize - 1]);
				DestroyElement(mSize - 1);
			}
		} else {
			if constexpr (MemcopyRealloc) {
				DestroyElement(Index);
				std::memmove(&mData[Index], &mData[mSize - 1], sizeof(element_type));
			} else {
				SwapElements(mData[Index], mData[mSize - 1]);
				DestroyElement(mSize - 1);
			}
		}
		--mSize;
		return;
	}

	FORCEINLINE void DestroyElement(index Index)
		requires(!FastDestruct)
	{
		Data()[Index].~element_type();
	}

	FORCEINLINE void DestroyElement(index Index)
		requires(FastDestruct)
	{
	}

	FORCEINLINE bool Reserve(index TargetCapacity) {
		if (mCapacity >= TargetCapacity) {
			return true;
		}
		if (mData && alloc_base::mAllocator.Expand(mData, TargetCapacity * sizeof(element_type))) {
			mCapacity = TargetCapacity;
			return true;
		}
		element_type* OldData = Data();
		element_type* NewData =
			(element_type*) alloc_base::mAllocator.Allocate(TargetCapacity * sizeof(element_type));
		if (NewData) {
			std::memcpy(NewData, OldData, mSize * sizeof(element_type));
			alloc_base::mAllocator.Free(mData);
			mData = NewData;
			mCapacity = TargetCapacity;
			return true;
		}

		return false;
	}

	FORCEINLINE void Clear(bool Deallocate = true) {
		if (!FastDestruct) {
			for (index i = 0; i < mSize; ++i) {
				DestroyElement(i);
			}
		}
		if (mData && Deallocate) {
			alloc_base::mAllocator.Free(mData);
			mData = nullptr;
			mCapacity = StackSize;
		}
		mSize = 0;
	}

	FORCEINLINE iter begin() {
		if constexpr (StackSize > 0) {
			if (mCapacity <= StackSize) {
				return iter(reinterpret_cast<element_type*>(this->mStackStorage));
			}
		}
		return iter(mData);
	}

	FORCEINLINE iter end() {
		if constexpr (StackSize > 0) {
			if (mCapacity <= StackSize) {
				return iter(reinterpret_cast<element_type*>(this->mStackStorage) + mSize);
			}
		}
		return iter(mData + mSize);
	}

	FORCEINLINE const_iter begin() const {
		if constexpr (StackSize > 0) {
			if (mCapacity <= StackSize) {
				return const_iter(reinterpret_cast<const element_type*>(this->mStackStorage));
			}
		}
		return const_iter(mData);
	}

	FORCEINLINE const_iter end() const {
		if constexpr (StackSize > 0) {
			if (mCapacity <= StackSize) {
				return const_iter(reinterpret_cast<const element_type*>(
					reinterpret_cast<const element_type*>(this->mStackStorage) + mSize));
			}
		}
		return const_iter(mData + mSize);
	}

	FORCEINLINE index FindFirst(const element_type& Value) {
		for (index i = 0; i < mSize; ++i) {
			if (Value == mData[i]) {
				return i;
			}
		}
		return InvalidIndex;
	}

	FORCEINLINE index FindLast(const element_type& Value) {
		for (index i = mSize - 1;; --i) {
			if (Value == mData[i]) {
				return i;
			}
			if (i == 0) {
				break;
			}
		}
		return InvalidIndex;
	}

	template <typename predicate_type>
	FORCEINLINE index FindFirstByPredicate(const predicate_type& Predicate) {
		for (index i = 0; i < mSize; ++i) {
			if (Predicate(mData[i])) {
				return i;
			}
		}
		return InvalidIndex;
	}

	template <typename predicate_type>
	FORCEINLINE index FindLastByPredicate(const predicate_type& Predicate) {
		for (index i = mSize - 1;; --i) {
			if (Predicate(mData[i])) {
				return i;
			}
			if (i == 0) {
				break;
			}
		}
		return InvalidIndex;
	}

	template <typename less_type = default_less_op>
	FORCEINLINE void Sort(less_type LessOp = default_less_op{}) {
		Quicksort(mData, mData + mSize, LessOp);
	}

	template <typename less_type = default_less_op>
	FORCEINLINE void StableSort(less_type LessOp = default_less_op{}) {
		MergeSort(mData, mData + mSize, LessOp);
	}

	template <typename less_type>
	void Quicksort(element_type* Begin, element_type* End, less_type LessOp) {
		struct sort_partition {
			element_type* mFirst;
			element_type* mLast;
		};

		sort_partition Stack[32];
		// slower but more general variant
		// index_type MaxDepth = LogOfTwoCeil(mSize) + 1;
		// auto* Stack = (sort_partition*)alloca(MaxDepth * sizeof(sort_partition));

		Stack[0] = {Begin, End - 1};
		sort_partition Current;
		for (sort_partition* StackTop = Stack; StackTop >= Stack; --StackTop) {
			Current = *StackTop;
			bool Loop;
			do {
				Loop = false;
				index Count = Current.mLast - Current.mFirst + 1;
				if (Count <= 32) {
					InsertionSort(Current.mFirst, Current.mLast + 1, LessOp);
					continue;
				}
				SwapElements(*(Current.mFirst), (Current.mFirst[Count / 2]));
				element_type* Left = Current.mFirst;
				element_type* Right = Current.mLast + 1;
				for (;;) {
					while (++Left <= Current.mLast && !LessOp(*(Current.mFirst), *Left))
						;
					while (--Right > Current.mFirst && !LessOp(*Right, *(Current.mFirst)))
						;
					if (Left > Right) {
						break;
					}
					SwapElements(*Left, *Right);
				}
				SwapElements(*(Current.mFirst), *Right);
				if (Right - Current.mFirst - 1 >= Current.mLast - Left) {
					if (Current.mFirst + 1 < Right) {
						StackTop->mFirst = Current.mFirst;
						StackTop->mLast = Right - 1;
						++StackTop;
					}
					if (Current.mLast > Left) {
						Current.mFirst = Left;
						Loop = true;
					}
				} else {
					if (Current.mLast > Left) {
						StackTop->mFirst = Left;
						StackTop->mLast = Current.mLast;
						++StackTop;
					}
					if (Current.mFirst + 1 < Right) {
						Current.mLast = Right - 1;
						Loop = true;
					}
				}
			} while (Loop);
		}
	}

	template <typename less_type>
	void MergeSort(element_type* Begin, element_type* End, less_type LessOp) {
		// TODO
	}

	template <typename less_type>
	FORCEINLINE void InsertionSort(element_type* Begin, element_type* End, less_type LessOp) {
		if (Begin == End) {
			return;
		}
		for (element_type* Mid = Begin + 1; Mid != End; ++Mid) {
			if constexpr (MemcopyRealloc) {
				element_type* Hole = Mid;
				alignas(element_type) u8 ValueBuffer[sizeof(element_type)]{};
				auto* Value = (element_type*) (&ValueBuffer);
				memcpy(Value, Mid, sizeof(element_type));
				for (element_type* Prev = Hole - 1; LessOp(*Value, *Prev) && Prev >= Begin;
					 --Prev) {
					memcpy(Hole, Prev, sizeof(element_type));
					Hole = Prev;
				}
				memcpy(Hole, Value, sizeof(element_type));
			} else {
				element_type* Hole = Mid;
				element_type Value{std::move(*Mid)};
				for (element_type* Prev = Hole - 1; Prev >= Begin && LessOp(Value, *Prev); --Prev) {
					*Hole = std::move(*Prev);
					Hole = Prev;
				}
				*Hole = std::move(Value);
			}
		}
	}

	FORCEINLINE void ShrinkToFit() {
		// TODO
	}

	FORCEINLINE bool EnsureCapacity(const index NewCapacity) {
		if (mCapacity == 0 && NewCapacity != 0) {
			constexpr index InitialCapacity = 4;
			const index TargetCapacity = Max(InitialCapacity, NewCapacity);
			mData = (element_type*) alloc_base::mAllocator.Allocate(
				TargetCapacity * sizeof(element_type));
			if (mData) {
				mCapacity = TargetCapacity;
			}
			return mData;

		} else if (mCapacity < NewCapacity) {
			const index DoubledCapacity = 1 << (LogOfTwoCeil(mCapacity) + 1);
			return Reserve(
				Min(MaxSingleCapacityIncrease + mCapacity, Max(DoubledCapacity, NewCapacity)));
		}
		return true;
	}

	FORCEINLINE void ShiftBackOne(index From) {
		element_type* ActualData = Data();
		if constexpr (MemcopyRealloc) {
			std::memmove(
				&ActualData[From - 1], &ActualData[From], (mSize - From) * sizeof(element_type));
		} else {
			if (From < mSize) {
				new (&ActualData[From - 1]) element_type(std::move(ActualData[From]));
				for (++From; From != mSize; ++From) {
					ActualData[From - 1] = std::move(ActualData[From]);
				}
				DestroyElement(mSize - 1);
			}
		}
	}

	FORCEINLINE void ShiftForwardOne(index From) {
		element_type* ActualData = Data();
		if constexpr (MemcopyRealloc) {
			std::memmove(
				&ActualData[From + 1], &ActualData[From], (mSize - From) * sizeof(element_type));
		} else {
			if (From < mSize) {
				index Source = mSize;
				new (&ActualData[Source]) element_type(std::move(ActualData[Source - 1]));
				for (--Source; Source != From; --Source) {
					ActualData[Source] = std::move(ActualData[Source - 1]);
				}
				DestroyElement(From);
			}
		}
	}

	FORCEINLINE void CopyConstructElements(
		element_type* Dest,
		const element_type* Source,
		const index Count) {
		if constexpr (FastCopy) {
			std::memcpy(Dest, Source, Count * sizeof(element_type));
		} else {
			for (index i = 0; i < Count; ++i) {
				new (&Dest[i]) element_type(Source[i]);
			}
		}
	}

	FORCEINLINE void SwapElements(element_type& Rhs, element_type& Lhs) {
		if constexpr (MemcopyRealloc) {
			Memswap(Rhs, Lhs);
		} else {
			std::swap(Rhs, Lhs);
		}
	}

	FORCEINLINE void GrabFromOther(dyn_array&& Other) {
		if constexpr (StackSize > 0) {
			if (Other.mCapacity <= StackSize) {
				std::memcpy(
					reinterpret_cast<element_type*>(this->mStackStorage),
					reinterpret_cast<element_type*>(Other.mStackStorage),
					Other.mSize);
			} else {
				mData = Other.mData;
			}
		} else {
			mData = Other.mData;
		}
		mSize = Other.mSize;
		mCapacity = Other.mCapacity;
		Other.mData = nullptr;
		Other.mSize = 0;
		Other.mCapacity = StackSize;
	}
};