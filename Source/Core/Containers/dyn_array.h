#pragma once

#include "basic.h"
#include "Templates/less.h"
#include "Templates/concepts.h"
#include "Math/math.h"
#include "Memory/memory.h"
#include "array_iter.h"
#include "span.h"

#include <initializer_list>
#include <limits>

template <typename element_type, index InStackSize>
struct array_storage {
	alignas(element_type) u8 Data[InStackSize * sizeof(element_type)];
};

template <typename element_type>
struct array_storage<element_type, 0> {};

template <typename element_type, typename allocator_type = default_allocator, index InStackSize = 0>
struct dyn_array : allocator_instance<allocator_type> {
private:
	index Size{0};
	index Capacity{InStackSize};

	union {
		element_type* Data{nullptr};
		array_storage<element_type, InStackSize> StackStorage;
	};

public:
	constexpr static bool MemcopyRelocatable = true;

	using iter = array_iter<dyn_array, iterator_constness::non_constant>;
	using const_iter = array_iter<dyn_array, iterator_constness::constant>;
	using value_type = element_type;
	using alloc_base = allocator_instance<allocator_type>;
	constexpr static index StackSize = InStackSize;
	constexpr static bool FastCopy = trivially_copyable<element_type>;
	constexpr static bool FastDestruct = trivially_destructible<element_type>;
	constexpr static bool MemcopyRealloc = memcopy_relocatable<element_type>;

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
				return reinterpret_cast<element_type*>(StackStorage.Data);
			}
		}
		return Data;
	}

	FORCEINLINE const element_type* GetData() const {
		if constexpr (StackSize > 0) {
			if (Capacity <= StackSize) {
				return reinterpret_cast<const element_type*>(StackStorage.Data);
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

	FORCEINLINE bool HasAllocation() const {
		return StackSize < Capacity && Data;
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
		if (HasAllocation() && alloc_base::Allocator.Expand(Data, TargetCapacity * sizeof(element_type))) {
			Capacity = TargetCapacity;
			return true;
		}
		element_type* OldData = GetData();
		element_type* NewData = (element_type*) alloc_base::Allocator.Allocate(TargetCapacity * sizeof(element_type));
		if (NewData) {
			std::memcpy(NewData, OldData, Size * sizeof(element_type));
			if (HasAllocation()) {
				alloc_base::Allocator.Free(Data);
			}
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
		if (HasAllocation() && ClearType == container_clear_type::deallocate) {
			alloc_base::Allocator.Free(Data);
			Data = nullptr;
			Capacity = StackSize;
		}
		Size = 0;
	}

	FORCEINLINE iter begin() {
		if constexpr (StackSize > 0) {
			if (Capacity <= StackSize) {
				return iter(reinterpret_cast<element_type*>(StackStorage.Data));
			}
		}
		return iter(Data);
	}

	FORCEINLINE iter end() {
		if constexpr (StackSize > 0) {
			if (Capacity <= StackSize) {
				return iter(reinterpret_cast<element_type*>(StackStorage.Data) + Size);
			}
		}
		return iter(Data + Size);
	}

	FORCEINLINE const_iter begin() const {
		if constexpr (StackSize > 0) {
			if (Capacity <= StackSize) {
				return const_iter(reinterpret_cast<const element_type*>(StackStorage.Data));
			}
		}
		return const_iter(Data);
	}

	FORCEINLINE const_iter end() const {
		if constexpr (StackSize > 0) {
			if (Capacity <= StackSize) {
				return const_iter(reinterpret_cast<const element_type*>(
					reinterpret_cast<const element_type*>(StackStorage.Data) + Size));
			}
		}
		return const_iter(Data + Size);
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
			return Reserve(math::Max(DoubledCapacity, NewCapacity));
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
					reinterpret_cast<element_type*>(StackStorage.Data),
					reinterpret_cast<element_type*>(Other.StackStorage.Data),
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