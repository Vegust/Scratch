#pragma once

#include "basic.h"
#include "Templates/less.h"
#include "Templates/concepts.h"
#include "Templates/traits.h"
#include "Math/math.h"
#include "Memory/memory.h"
#include "array_iter.h"

#include <initializer_list>
#include <limits>

template <typename element_type, u64 StackSize>
struct array_storage {
public:
	array_storage() {
	}

protected:
	NO_UNIQUE_ADDRESS alignas(element_type) u8 StackStorage[StackSize * sizeof(element_type)] = {0};
};

template <typename element_type>
class array_storage<element_type, 0> {};

template <typename element_type, typename allocator_type = default_allocator, index InStackSize = 0>
struct dyn_array : array_storage<element_type, InStackSize>,
				   trait_memcopy_relocatable,
				   allocator_instance<allocator_type> {
private:
	element_type* Data{nullptr};
	index Size{0};
	index Capacity{0};

public:
	using iter = array_iter<dyn_array, iterator_constness::non_constant>;
	using const_iter = array_iter<dyn_array, iterator_constness::constant>;
	using value_type = element_type;
	using alloc_base = allocator_instance<allocator_type>;
	constexpr static index StackSize = InStackSize;
	constexpr static bool FastCopy = trivially_copyable<element_type>;
	constexpr static bool FastDestruct = trivially_destructible<element_type>;
	constexpr static bool MemcopyRealloc = memcopy_relocatable<element_type>;
	// 10 MB, completely arbitrary for now TODO: this number should be backed by something I guess
	constexpr static index MaxSingleByteSizeIncrease = 1024 * 1024 * 10;
	constexpr static index MaxSingleCapacityIncrease = math::Max(1, MaxSingleByteSizeIncrease / sizeof(element_type));

	dyn_array() = default;

	FORCEINLINE ~dyn_array() {
		Clear();
	}

	FORCEINLINE explicit dyn_array(index InitialCapacity) {
		EnsureCapacity(InitialCapacity);
	}

	FORCEINLINE explicit dyn_array(element_type* InData, index InSize) {
		Data = InData;
		Capacity = InSize;
		Size = InSize;
	}

	FORCEINLINE explicit dyn_array(const element_type* InData, index InSize) {
		EnsureCapacity(InSize);
		CopyConstructElements(GetData(), InData, InSize);
		Size = InSize;
	}

	FORCEINLINE dyn_array(std::initializer_list<element_type> InitList) : dyn_array(InitList.begin(), InitList.size()) {
	}

	FORCEINLINE dyn_array(const dyn_array& Other) {
		EnsureCapacity(Other.Size);
		CopyConstructElements(GetData(), Other.GetData(), Other.Size);
		Size = Other.Size;
	}

	FORCEINLINE dyn_array(dyn_array&& Other) noexcept {
		GrabFromOther(std::move(Other));
	}

	FORCEINLINE bool operator==(const dyn_array& Other) const {
		if (Size != Other.Size) {
			return false;
		}
		for (index i = 0; i < Size; ++i) {
			if (Other[i] != operator[](i)) {
				return false;
			}
		}
		return true;
	}

	FORCEINLINE dyn_array& AppendInplace(const dyn_array& Other) {
		EnsureCapacity(Size + Other.Size);
		CopyConstructElements(end(), Other.GetData(), Other.Size);
		Size = Size + Other.Size;
		return *this;
	}

	FORCEINLINE dyn_array& AppendInplace(dyn_array&& Other) {
		EnsureCapacity(Size + Other.Size);
		std::memcpy(end(), Other.GetData(), Other.Size * sizeof(element_type));
		Size = Size + Other.Size;
		return *this;
	}

	FORCEINLINE dyn_array& operator=(std::initializer_list<element_type> InitList) {
		Clear(false);
		EnsureCapacity(InitList.size());
		CopyConstructElements(GetData(), InitList.begin(), InitList.size());
		Size = InitList.size();
		return *this;
	}

	FORCEINLINE dyn_array& operator=(dyn_array&& Other) noexcept {
		if (&Other == this) {
			return *this;
		}
		Clear(container_clear_type::dont_deallocate);
		GrabFromOther(std::move(Other));

		return *this;
	}

	FORCEINLINE dyn_array& operator=(const dyn_array& Other) {
		if (&Other == this) {
			return *this;
		}

		Clear(container_clear_type::dont_deallocate);
		EnsureCapacity(Other.Size);
		CopyConstructElements(GetData(), Other.GetData(), Other.Size);
		Size = Other.Size;

		return *this;
	}

	FORCEINLINE element_type* GetData() {
		if constexpr (StackSize > 0) {
			if (Capacity <= StackSize) {
				return reinterpret_cast<element_type*>(this->StackStorage);
			}
		}
		return Data;
	}

	FORCEINLINE const element_type* GetData() const {
		if constexpr (StackSize > 0) {
			if (Capacity <= StackSize) {
				return reinterpret_cast<const element_type*>(this->StackStorage);
			}
		}
		return Data;
	}

	FORCEINLINE index GetCapacity() const {
		return Capacity;
	}

	FORCEINLINE index GetSize() const {
		return Size;
	}

	FORCEINLINE element_type& operator[](const index Index) {
		return GetData()[Index];
	}

	FORCEINLINE const element_type& operator[](const index Index) const {
		return GetData()[Index];
	}

	template <typename... TArgs>
	FORCEINLINE index Emplace(TArgs&&... Args) {
		return EmplaceAt(Size, std::forward<TArgs>(Args)...);
	}

	FORCEINLINE index Add(const element_type& Element) {
		return AddAt(Size, Element);
	}

	FORCEINLINE index AddAt(const index Index, const element_type& Element) {
		if (EnsureCapacity(Size + 1)) {
			ShiftForwardOne(Index);	   // TODO: potentially 1 redundant memmove
			new (&(GetData()[Index])) element_type(Element);
			++Size;
			return Index;
		}
		return InvalidIndex;
	}

	template <typename... TArgs>
	FORCEINLINE index EmplaceAt(const index Index, TArgs&&... Args) {
		if (EnsureCapacity(Size + 1)) {
			ShiftForwardOne(Index);	   // TODO: potentially 1 redundant move
			new (&(GetData()[Index])) element_type(std::forward<TArgs>(Args)...);
			++Size;
			return Index;
		}
		return InvalidIndex;
	}

	FORCEINLINE void RemoveAt(const index Index) {
		DestroyElement(Index);
		ShiftBackOne(Index + 1);
		--Size;
	}

	FORCEINLINE void RemoveAtSwap(const index Index) {
		if constexpr (StackSize > 0) {
			element_type* ActualData = GetData();
			if constexpr (MemcopyRealloc) {
				DestroyElement(Index);
				std::memmove(&ActualData[Index], &ActualData[Size - 1], sizeof(element_type));
			} else {
				SwapElements(ActualData[Index], ActualData[Size - 1]);
				DestroyElement(Size - 1);
			}
		} else {
			if constexpr (MemcopyRealloc) {
				DestroyElement(Index);
				std::memmove(&Data[Index], &Data[Size - 1], sizeof(element_type));
			} else {
				SwapElements(Data[Index], Data[Size - 1]);
				DestroyElement(Size - 1);
			}
		}
		--Size;
		return;
	}

	FORCEINLINE void DestroyElement(index Index)
		requires(!FastDestruct)
	{
		GetData()[Index].~element_type();
	}

	FORCEINLINE void DestroyElement(index Index)
		requires(FastDestruct)
	{
	}

	// Reserve() allocates exactly provided amount (contrary to EnsureCapacity())
	FORCEINLINE bool Reserve(index TargetCapacity) {
		if (Capacity >= TargetCapacity) {
			return true;
		}
		if (Data && alloc_base::Allocator.Expand(Data, TargetCapacity * sizeof(element_type))) {
			Capacity = TargetCapacity;
			return true;
		}
		element_type* OldData = GetData();
		element_type* NewData = (element_type*) alloc_base::Allocator.Allocate(TargetCapacity * sizeof(element_type));
		if (NewData) {
			std::memcpy(NewData, OldData, Size * sizeof(element_type));
			alloc_base::Allocator.Free(Data);
			Data = NewData;
			Capacity = TargetCapacity;
			return true;
		}

		return false;
	}

	FORCEINLINE void Clear(container_clear_type ClearType = container_clear_type::deallocate) {
		if (!FastDestruct) {
			for (index i = 0; i < Size; ++i) {
				DestroyElement(i);
			}
		}
		if (Data && ClearType == container_clear_type::deallocate) {
			alloc_base::Allocator.Free(Data);
			Data = nullptr;
			Capacity = StackSize;
		}
		Size = 0;
	}

	FORCEINLINE iter begin() {
		if constexpr (StackSize > 0) {
			if (Capacity <= StackSize) {
				return iter(reinterpret_cast<element_type*>(this->StackStorage));
			}
		}
		return iter(Data);
	}

	FORCEINLINE iter end() {
		if constexpr (StackSize > 0) {
			if (Capacity <= StackSize) {
				return iter(reinterpret_cast<element_type*>(this->StackStorage) + Size);
			}
		}
		return iter(Data + Size);
	}

	FORCEINLINE const_iter begin() const {
		if constexpr (StackSize > 0) {
			if (Capacity <= StackSize) {
				return const_iter(reinterpret_cast<const element_type*>(this->StackStorage));
			}
		}
		return const_iter(Data);
	}

	FORCEINLINE const_iter end() const {
		if constexpr (StackSize > 0) {
			if (Capacity <= StackSize) {
				return const_iter(
					reinterpret_cast<const element_type*>(reinterpret_cast<const element_type*>(this->StackStorage) + Size));
			}
		}
		return const_iter(Data + Size);
	}

	FORCEINLINE index FindFirst(const element_type& Value) {
		for (index i = 0; i < Size; ++i) {
			if (Value == Data[i]) {
				return i;
			}
		}
		return InvalidIndex;
	}

	FORCEINLINE index FindLast(const element_type& Value) {
		for (index i = Size - 1;; --i) {
			if (Value == Data[i]) {
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
		for (index i = 0; i < Size; ++i) {
			if (Predicate(Data[i])) {
				return i;
			}
		}
		return InvalidIndex;
	}

	template <typename predicate_type>
	FORCEINLINE index FindLastByPredicate(const predicate_type& Predicate) {
		for (index i = Size - 1;; --i) {
			if (Predicate(Data[i])) {
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
		Quicksort(Data, Data + Size, LessOp);
	}

	template <typename less_type = default_less_op>
	FORCEINLINE void StableSort(less_type LessOp = default_less_op{}) {
		MergeSort(Data, Data + Size, LessOp);
	}

	template <typename less_type>
	void Quicksort(element_type* Begin, element_type* End, less_type LessOp) {
		struct sort_partition {
			element_type* First;
			element_type* Last;
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
				index Count = Current.Last - Current.First + 1;
				if (Count <= 32) {
					InsertionSort(Current.First, Current.Last + 1, LessOp);
					continue;
				}
				SwapElements(*(Current.First), (Current.First[Count / 2]));
				element_type* Left = Current.First;
				element_type* Right = Current.Last + 1;
				for (;;) {
					while (++Left <= Current.Last && !LessOp(*(Current.First), *Left))
						;
					while (--Right > Current.First && !LessOp(*Right, *(Current.First)))
						;
					if (Left > Right) {
						break;
					}
					SwapElements(*Left, *Right);
				}
				SwapElements(*(Current.First), *Right);
				if (Right - Current.First - 1 >= Current.Last - Left) {
					if (Current.First + 1 < Right) {
						StackTop->First = Current.First;
						StackTop->Last = Right - 1;
						++StackTop;
					}
					if (Current.Last > Left) {
						Current.First = Left;
						Loop = true;
					}
				} else {
					if (Current.Last > Left) {
						StackTop->First = Left;
						StackTop->Last = Current.Last;
						++StackTop;
					}
					if (Current.First + 1 < Right) {
						Current.Last = Right - 1;
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
				for (element_type* Prev = Hole - 1; LessOp(*Value, *Prev) && Prev >= Begin; --Prev) {
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

	// EnsureCapacity() allocates to next power of 2 (contrary to Reserve())
	FORCEINLINE bool EnsureCapacity(const index NewCapacity) {
		if (Capacity == 0 && NewCapacity != 0) {
			constexpr index InitialCapacity = 4;
			const index TargetCapacity = math::Max(InitialCapacity, NewCapacity);
			Data = (element_type*) alloc_base::Allocator.Allocate(TargetCapacity * sizeof(element_type));
			if (Data) {
				Capacity = TargetCapacity;
			}
			return Data;

		} else if (Capacity < NewCapacity) {
			const index DoubledCapacity = 1 << (math::LogOfTwoCeil(Capacity) + 1);
			return Reserve(math::Min(MaxSingleCapacityIncrease + Capacity, math::Max(DoubledCapacity, NewCapacity)));
		}
		return true;
	}

	FORCEINLINE void ShiftBackOne(index From) {
		element_type* ActualData = GetData();
		if constexpr (MemcopyRealloc) {
			std::memmove(&ActualData[From - 1], &ActualData[From], (Size - From) * sizeof(element_type));
		} else {
			if (From < Size) {
				new (&ActualData[From - 1]) element_type(std::move(ActualData[From]));
				for (++From; From != Size; ++From) {
					ActualData[From - 1] = std::move(ActualData[From]);
				}
				DestroyElement(Size - 1);
			}
		}
	}

	FORCEINLINE void ShiftForwardOne(index From) {
		element_type* ActualData = GetData();
		if constexpr (MemcopyRealloc) {
			std::memmove(&ActualData[From + 1], &ActualData[From], (Size - From) * sizeof(element_type));
		} else {
			if (From < Size) {
				index Source = Size;
				new (&ActualData[Source]) element_type(std::move(ActualData[Source - 1]));
				for (--Source; Source != From; --Source) {
					ActualData[Source] = std::move(ActualData[Source - 1]);
				}
				DestroyElement(From);
			}
		}
	}

	FORCEINLINE void CopyConstructElements(element_type* Dest, const element_type* Source, const index Count) {
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
			if (Other.Capacity <= StackSize) {
				std::memcpy(
					reinterpret_cast<element_type*>(this->StackStorage),
					reinterpret_cast<element_type*>(Other.StackStorage),
					Other.Size);
			} else {
				Data = Other.Data;
			}
		} else {
			Data = Other.Data;
		}
		Size = Other.Size;
		Capacity = Other.Capacity;
		Other.Data = nullptr;
		Other.Size = 0;
		Other.Capacity = StackSize;
	}

	// NOTE: ugly
	FORCEINLINE void OverwriteSize(const index NewSize) {
		Size = NewSize;
	}
};