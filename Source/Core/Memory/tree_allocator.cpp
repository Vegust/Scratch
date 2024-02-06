#include "tree_allocator.h"

#include "Math/math.h"
#include "Containers/array.h"
#include "Containers/dyn_array.h"
#include "Containers/rb_set.h"

struct tree_allocator_impl {
	struct pool {
		struct pool_header {
			// padding so mAllocIndex for pool header and tree header are identical
			u32 : 32;
			bool : 8;
			u16 AllocIndex{0};
		};

		constexpr static u32 FirstAllocationSize = 256;
		dyn_array<void*, malloc_allocator> Allocations{};
		void* FreeElement{nullptr};
		u32 NextIndex{0};

		void* Allocate(u32 Size, u32 AllocIndex) {
			Size = Size + sizeof(pool_header);
			if (FreeElement) {
				void* Element = FreeElement;
				FreeElement = *(void**) Element;
				return Element;
			}
			u32 LastAllocationSize = FirstAllocationSize * (1 << (Allocations.GetSize() - 1));
			if (NextIndex != 0 && NextIndex < LastAllocationSize) {
				void* LastAllocation = Allocations[Allocations.GetSize() - 1];
				pool_header* Header = (pool_header*) ((u8*) LastAllocation + ((NextIndex++) * Size));
				Header->AllocIndex = AllocIndex;
				return Header + 1;
			} else {
				void* NewAllocation = MemoryMalloc(Size * FirstAllocationSize * (1 << Allocations.GetSize()));
				Allocations.Add(NewAllocation);
				NextIndex = 1;
				((pool_header*) NewAllocation)->AllocIndex = AllocIndex;
				return (u8*) NewAllocation + sizeof(pool_header);
			}
		}

		void Free(void* Ptr) {
			*(void**) Ptr = FreeElement;
			FreeElement = Ptr;
		}

		void Clear() {
			for (void* Allocation : Allocations) {
				MemoryFree(Allocation);
			}
			Allocations.Clear();
			FreeElement = nullptr;
			NextIndex = 0;
		}
	};

	struct header {
		u32 Size{0};
		u32 Prev{InvalidIndex};
		u32 Next{InvalidIndex};
		bool Occupied{false};
		u16 AllocIndex{0};

		FORCEINLINE header* Occupy(u32 NewSize) {
			if (Size < (NewSize + HeaderSize + MinAllocSize)) {
				// Not enough space for leftover header
				return nullptr;
			}
			header* LeftoverHeader = this + NewSize + HeaderSize;
			LeftoverHeader->AllocIndex = AllocIndex;
			u32 LeftoverHeaderIndex = GetIndex(LeftoverHeader);
			LeftoverHeader->Size = Size - NewSize - HeaderSize;
			LeftoverHeader->Prev = GetIndex(this);
			LeftoverHeader->Occupied = false;
			if (Next != InvalidIndex) {
				header* NextHeader = GetHeader(Next, AllocIndex);
				NextHeader->Prev = LeftoverHeaderIndex;
				LeftoverHeader->Next = Next;
			} else {
				LeftoverHeader->Next = InvalidIndex;
			}
			Next = LeftoverHeaderIndex;
			Size = NewSize;
			return LeftoverHeader;
		}

		FORCEINLINE void* Data() {
			return this + HeaderSize;
		}

		FORCEINLINE static header* FromDataPtr(void* data_ptr) {
			return (header*) (data_ptr) -HeaderSize;
		}
	};

	struct free_header_data {
		header* Header{nullptr};
		u32 Size{0};
	};

	struct free_header_less_op {
		FORCEINLINE constexpr static bool Less(const free_header_data& Lhs, const free_header_data& Rhs) {
			if (Lhs.Size == Rhs.Size) {
				return (u64) Lhs.Header < (u64) Rhs.Header;
			}
			return Lhs.Size < Rhs.Size;
		}
	};

	struct free_header_equals_op {
		FORCEINLINE constexpr static bool Equals(const free_header_data& Lhs, const free_header_data& Rhs) {
			return Lhs.Header == Rhs.Header;
		}
	};

	static constexpr index Granularity = 16;
	constexpr static u32 NumPools = 4;
	// all sizes in this file are in multiples of 16 (Granularity)
	constexpr static u32 HeaderSize = 1;
	constexpr static u32 MinAllocSize = NumPools + 1;
	constexpr static u32 BlockSize = 1024;					   // * 16 = 16384 bytes
	constexpr static u32 MaxDoublingSize = 1024 * 1024 * 8;	   // 128 MB after that we allocate only
															   // as much as needed
	dyn_array<void*, malloc_allocator> Allocations{};
	// set allocates nodes using this allocator but because nodes are small they are allocated
	// using pools so there is no recursion
	rb_set<free_header_data, free_header_less_op, free_header_equals_op> FreeHeaders{};
	array<pool, NumPools> Pools{};	  // 16,32,48,64
	s32 NumBlocks = 0;

	FORCEINLINE static u32 GranularSize(u64 Size) {
		return Size > 0 ? (Size >> 4) + 1 : 0;
	}

	FORCEINLINE static u32 GetIndex(const header* Header);

	FORCEINLINE static header* GetHeader(u32 HeaderIndex, u16 AllocationIndex);

	~tree_allocator_impl() {
		Clear();
	}

	void Clear() {
		FreeHeaders.Clear();
		for (auto& Allocation : Allocations) {
			MemoryFree(Allocation);
		}
		Allocations.Clear();
		for (auto& Pool : Pools) {
			Pool.Clear();
		}
		// mFreeHeaders.mAllocator.ClearPools();
		NumBlocks = 0;
	}

	// Requested size in multiples of 16 bytes
	void* Allocate(u32 RequestedSize) {
		if (RequestedSize == 0) {
			return nullptr;
		}
		if ((RequestedSize - 1) < NumPools) {
			return Pools[RequestedSize - 1].Allocate(RequestedSize * 16, RequestedSize - 1);
		}
		free_header_data wanted_header;
		wanted_header.Size = RequestedSize;
		free_header_data* FoundData = FreeHeaders.UpperBound(wanted_header);
		if (FoundData) {
			header* FoundHeader = FoundData->Header;
			FreeHeaders.RemoveByPtr(FoundData);
			header* Leftovers = FoundHeader->Occupy(RequestedSize);
			FoundHeader->Occupied = true;
			if (Leftovers) {
				FreeHeaders.Add(free_header_data{Leftovers, Leftovers->Size});
				NumBlocks++;
			}
			return FoundHeader->Data();
		}
		u64 NewSize = math::Min(MaxDoublingSize, (u64) BlockSize * (1 << Allocations.GetSize()));
		if (NewSize < (RequestedSize + HeaderSize)) {
			NewSize = RequestedSize + HeaderSize;
		}
		void* NewAllocation = MemoryMalloc(NewSize * Granularity);
		if (!NewAllocation) {
			return nullptr;
		}
		Allocations.Add(NewAllocation);
		header* NewBlockHeader = (header*) (NewAllocation);
		NewBlockHeader->Occupied = true;
		NewBlockHeader->Prev = InvalidIndex;
		NewBlockHeader->Next = InvalidIndex;
		NewBlockHeader->Size = NewSize - HeaderSize;
		NewBlockHeader->AllocIndex = NumPools + Allocations.GetSize() - 1;
		NumBlocks++;
		header* Leftovers = NewBlockHeader->Occupy(RequestedSize);
		if (Leftovers) {
			FreeHeaders.Add(free_header_data{Leftovers, Leftovers->Size});
			NumBlocks++;
		}
		return NewBlockHeader->Data();
	}

	bool Expand(void* Ptr, u32 RequestedSize) {
		header* Header = header::FromDataPtr(Ptr);
		if (Header->AllocIndex < NumPools) {
			return false;
		}
		if (Header->Size >= RequestedSize) {
			return true;
		}
		if (Header->Next != InvalidIndex) {
			header* NextHeader = GetHeader(Header->Next, Header->AllocIndex);
			if (!NextHeader->Occupied) {
				u32 ExtraSize = RequestedSize - Header->Size - HeaderSize;
				if (NextHeader->Size >= ExtraSize) {
					FreeHeaders.Remove(free_header_data{NextHeader, NextHeader->Size});
					header* Leftovers = NextHeader->Occupy(ExtraSize);
					Header->Size += NextHeader->Size + HeaderSize;
					Header->Next = NextHeader->Next;
					if (Leftovers) {
						u32 HeaderIndex = GetIndex(Header);
						Leftovers->Prev = HeaderIndex;
						FreeHeaders.Add(free_header_data{Leftovers, Leftovers->Size});
					} else {
						NumBlocks--;
						if (Header->Next != InvalidIndex) {
							header* NewNextHeader = GetHeader(Header->Next, Header->AllocIndex);
							u32 HeaderIndex = GetIndex(Header);
							NewNextHeader->Prev = HeaderIndex;
						}
					}
					return true;
				}
			}
		}
		return false;
	}

	void Free(void* Ptr) {
		// NOTE: this might be illegal with pool headers?
		header* FreedHeader = header::FromDataPtr(Ptr);
		u16 AllocIndex = FreedHeader->AllocIndex;
		if (AllocIndex < NumPools) {
			Pools[AllocIndex].Free(Ptr);
			return;
		}
		if (FreedHeader->Next != InvalidIndex) {
			header* NextHeader = GetHeader(FreedHeader->Next, FreedHeader->AllocIndex);
			if (!NextHeader->Occupied) {
				FreeHeaders.Remove(free_header_data{NextHeader, NextHeader->Size});
				NumBlocks--;
				FreedHeader->Size += NextHeader->Size + HeaderSize;
				FreedHeader->Next = NextHeader->Next;
				if (FreedHeader->Next != InvalidIndex) {
					header* NewNextHeader = GetHeader(FreedHeader->Next, FreedHeader->AllocIndex);
					u32 FreedHeaderIndex = GetIndex(FreedHeader);
					NewNextHeader->Prev = FreedHeaderIndex;
				}
			}
		}
		if (FreedHeader->Prev != InvalidIndex) {
			header* PrevHeader = GetHeader(FreedHeader->Prev, FreedHeader->AllocIndex);
			if (!PrevHeader->Occupied) {
				u32 PrevHeaderIndex = FreedHeader->Prev;
				FreeHeaders.Remove(free_header_data{PrevHeader, PrevHeader->Size});
				NumBlocks--;
				PrevHeader->Size += FreedHeader->Size + HeaderSize;
				PrevHeader->Next = FreedHeader->Next;
				if (PrevHeader->Next != InvalidIndex) {
					header* NextHeader = GetHeader(PrevHeader->Next, PrevHeader->AllocIndex);
					NextHeader->Prev = PrevHeaderIndex;
				}
				FreedHeader = PrevHeader;
			}
		}
		FreedHeader->Occupied = false;
		FreeHeaders.Add(free_header_data{FreedHeader, FreedHeader->Size});
	}

	static tree_allocator_impl& GetImpl();
};

static bool TreeAllocatorInitialized;
alignas(tree_allocator_impl) static array<u8, sizeof(tree_allocator_impl)> TreeAllocatorImplBytes;

tree_allocator_impl& tree_allocator_impl::GetImpl() {
	if (TreeAllocatorInitialized) {
		return *(tree_allocator_impl*) (&TreeAllocatorImplBytes);
	}
	tree_allocator_impl* Singleton = new (&TreeAllocatorImplBytes) tree_allocator_impl{};
	TreeAllocatorInitialized = true;
	return *Singleton;
}

tree_allocator_impl::header* tree_allocator_impl::GetHeader(u32 HeaderIndex, u16 AllocationIndex) {
	return (header*) ((*(tree_allocator_impl*) (&TreeAllocatorImplBytes)).Allocations[AllocationIndex - NumPools]) +
		   HeaderIndex;
}

u32 tree_allocator_impl::GetIndex(const tree_allocator_impl::header* Header) {
	return Header -
		   (header*) ((*(tree_allocator_impl*) (&TreeAllocatorImplBytes)).Allocations[Header->AllocIndex - NumPools]);
}

void tree_allocator::ClearStaticImpl() {
	tree_allocator_impl::GetImpl().Clear();
}

void* tree_allocator::StaticAllocateImpl(u64 Size, u8 Alignment) {
	return tree_allocator_impl::GetImpl().Allocate(tree_allocator_impl::GranularSize(Size));
}

void tree_allocator::StaticFreeImpl(void* Ptr) {
	tree_allocator_impl::GetImpl().Free(Ptr);
}

bool tree_allocator::StaticExpandImpl(void* Ptr, u64 NewSize) {
	return tree_allocator_impl::GetImpl().Expand(Ptr, tree_allocator_impl::GranularSize(NewSize));
}

static_assert(sizeof(tree_allocator_impl::header) == tree_allocator_impl::Granularity);
static_assert(
	sizeof(tree_allocator_impl::header) - offsetof(tree_allocator_impl::header, AllocIndex) ==
	sizeof(tree_allocator_impl::pool::pool_header) - offsetof(tree_allocator_impl::pool::pool_header, AllocIndex));