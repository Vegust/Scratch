#include "tree_allocator.h"

#include "Containers/array.h"
#include "Containers/dyn_array.h"
#include "Containers/rb_set.h"

struct tree_allocator_impl {
	struct pool {
		struct pool_header {
			// padding so mAllocIndex for pool header and tree header are identical
			u32 : 32;
			bool : 8;
			u16 mAllocIndex{0};
		};

		constexpr static u32 FirstAllocationSize = 256;
		dyn_array<void*, malloc_allocator> mAllocations{};
		void* mFreeElement{nullptr};
		u32 mNextIndex{0};

		void* Allocate(u32 Size, u32 AllocIndex) {
			Size = Size + sizeof(pool_header);
			if (mFreeElement) {
				void* Element = mFreeElement;
				mFreeElement = *(void**) Element;
				return Element;
			}
			u32 LastAllocationSize = FirstAllocationSize * (1 << (mAllocations.Size() - 1));
			if (mNextIndex != 0 && mNextIndex < LastAllocationSize) {
				void* LastAllocation = mAllocations[mAllocations.Size() - 1];
				pool_header* Header =
					(pool_header*) ((u8*) LastAllocation + ((mNextIndex++) * Size));
				Header->mAllocIndex = AllocIndex;
				return Header + 1;
			} else {
				void* NewAllocation =
					MemoryMalloc(Size * FirstAllocationSize * (1 << mAllocations.Size()));
				mAllocations.Add(NewAllocation);
				mNextIndex = 1;
				((pool_header*) NewAllocation)->mAllocIndex = AllocIndex;
				return (u8*) NewAllocation + sizeof(pool_header);
			}
		}

		void Free(void* Ptr) {
			*(void**) Ptr = mFreeElement;
			mFreeElement = Ptr;
		}

		void Clear() {
			for (void* Allocation : mAllocations) {
				MemoryFree(Allocation);
			}
			mAllocations.Clear();
			mFreeElement = nullptr;
			mNextIndex = 0;
		}
	};

	struct header {
		u32 mSize{0};
		u32 mPrev{InvalidIndex};
		u32 mNext{InvalidIndex};
		bool mOccupied{false};
		u16 mAllocIndex{0};

		FORCEINLINE header* Occupy(u32 NewSize) {
			if (mSize < (NewSize + HeaderSize + MinAllocSize)) {
				// Not enough space for leftover header
				return nullptr;
			}
			header* LeftoverHeader = this + NewSize + HeaderSize;
			LeftoverHeader->mAllocIndex = mAllocIndex;
			u32 LeftoverHeaderIndex = GetIndex(LeftoverHeader);
			LeftoverHeader->mSize = mSize - NewSize - HeaderSize;
			LeftoverHeader->mPrev = GetIndex(this);
			LeftoverHeader->mOccupied = false;
			if (mNext != InvalidIndex) {
				header* NextHeader = GetHeader(mNext, mAllocIndex);
				NextHeader->mPrev = LeftoverHeaderIndex;
				LeftoverHeader->mNext = mNext;
			} else {
				LeftoverHeader->mNext = InvalidIndex;
			}
			mNext = LeftoverHeaderIndex;
			mSize = NewSize;
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
		header* mHeader{nullptr};
		u32 mSize{0};
	};

	struct free_header_less_op {
		FORCEINLINE constexpr static bool Less(
			const free_header_data& Lhs,
			const free_header_data& Rhs) {
			if (Lhs.mSize == Rhs.mSize) {
				return (u64) Lhs.mHeader < (u64) Rhs.mHeader;
			}
			return Lhs.mSize < Rhs.mSize;
		}
	};

	struct free_header_equals_op {
		FORCEINLINE constexpr static bool Equals(
			const free_header_data& Lhs,
			const free_header_data& Rhs) {
			return Lhs.mHeader == Rhs.mHeader;
		}
	};

	static tree_allocator_impl Impl;
	static constexpr index Granularity = 16;
	constexpr static u32 NumPools = 4;
	// all sizes in this file are in multiples of 16 (Granularity)
	constexpr static u32 HeaderSize = 1;
	constexpr static u32 MinAllocSize = NumPools + 1;
	constexpr static u32 BlockSize = 1024;					   // * 16 = 16384 bytes
	constexpr static u32 MaxDoublingSize = 1024 * 1024 * 8;	   // 128 MB after that we allocate only
															   // as much as needed
	dyn_array<void*, malloc_allocator> mAllocations{};
	// set allocates nodes using this allocator but because nodes are small they are allocated
	// using pools so there is no recursion
	rb_set<
		free_header_data,
		free_header_less_op,
		free_header_equals_op>
		mFreeHeaders{};
	array<pool, NumPools> mPools{};	// 16,32,48,64
	s32 mNumBlocks = 0;

	FORCEINLINE static u32 GranularSize(u64 Size) {
		return Size > 0 ? (Size >> 4) + 1 : 0;
	}

	FORCEINLINE static u32 GetIndex(const header* Header) {
		return Header - (header*) (Impl.mAllocations[Header->mAllocIndex - NumPools]);
	}

	FORCEINLINE static header* GetHeader(u32 HeaderIndex, u16 AllocationIndex) {
		return (header*) (Impl.mAllocations[AllocationIndex - NumPools]) + HeaderIndex;
	}

	~tree_allocator_impl() {
		Clear();
	}

	void Clear() {
		mFreeHeaders.Clear();
		for (auto& Allocation : mAllocations) {
			MemoryFree(Allocation);
		}
		mAllocations.Clear();
		for (auto& Pool : mPools) {
			Pool.Clear();
		}
		//mFreeHeaders.mAllocator.ClearPools();
		mNumBlocks = 0;
	}

	// Requested size in multiples of 16 bytes
	void* Allocate(u32 RequestedSize) {
		if (RequestedSize == 0) {
			return nullptr;
		}
		if ((RequestedSize - 1) < NumPools) {
			return mPools[RequestedSize - 1].Allocate(RequestedSize * 16, RequestedSize - 1);
		}
		free_header_data wanted_header;
		wanted_header.mSize = RequestedSize;
		free_header_data* FoundData = mFreeHeaders.UpperBound(wanted_header);
		if (FoundData) {
			header* FoundHeader = FoundData->mHeader;
			mFreeHeaders.RemoveByPtr(FoundData);
			header* Leftovers = FoundHeader->Occupy(RequestedSize);
			FoundHeader->mOccupied = true;
			if (Leftovers) {
				mFreeHeaders.Add(free_header_data{Leftovers, Leftovers->mSize});
				mNumBlocks++;
			}
			return FoundHeader->Data();
		}
		u64 NewSize = Min(MaxDoublingSize, (u64) BlockSize * (1 << mAllocations.Size()));
		if (NewSize < (RequestedSize + HeaderSize)) {
			NewSize = RequestedSize + HeaderSize;
		}
		void* NewAllocation = MemoryMalloc(NewSize * Granularity);
		if (!NewAllocation) {
			return nullptr;
		}
		mAllocations.Add(NewAllocation);
		header* NewBlockHeader = (header*) (NewAllocation);
		NewBlockHeader->mOccupied = true;
		NewBlockHeader->mPrev = InvalidIndex;
		NewBlockHeader->mNext = InvalidIndex;
		NewBlockHeader->mSize = NewSize - HeaderSize;
		NewBlockHeader->mAllocIndex = NumPools + mAllocations.Size() - 1;
		mNumBlocks++;
		header* Leftovers = NewBlockHeader->Occupy(RequestedSize);
		if (Leftovers) {
			mFreeHeaders.Add(free_header_data{Leftovers, Leftovers->mSize});
			mNumBlocks++;
		}
		return NewBlockHeader->Data();
	}

	bool Expand(void* Ptr, u32 RequestedSize) {
		header* Header = header::FromDataPtr(Ptr);
		if (Header->mAllocIndex < NumPools) {
			return false;
		}
		if (Header->mSize >= RequestedSize) {
			return true;
		}
		if (Header->mNext != InvalidIndex) {
			header* NextHeader = GetHeader(Header->mNext, Header->mAllocIndex);
			if (!NextHeader->mOccupied) {
				u32 ExtraSize = RequestedSize - Header->mSize - HeaderSize;
				if (NextHeader->mSize >= ExtraSize) {
					mFreeHeaders.Remove(free_header_data{NextHeader, NextHeader->mSize});
					header* Leftovers = NextHeader->Occupy(ExtraSize);
					Header->mSize += NextHeader->mSize + HeaderSize;
					Header->mNext = NextHeader->mNext;
					if (Leftovers) {
						u32 HeaderIndex = GetIndex(Header);
						Leftovers->mPrev = HeaderIndex;
						mFreeHeaders.Add(free_header_data{Leftovers, Leftovers->mSize});
					} else {
						mNumBlocks--;
						if (Header->mNext != InvalidIndex) {
							header* NewNextHeader = GetHeader(Header->mNext, Header->mAllocIndex);
							u32 HeaderIndex = GetIndex(Header);
							NewNextHeader->mPrev = HeaderIndex;
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
		u16 AllocIndex = FreedHeader->mAllocIndex;
		if (AllocIndex < NumPools) {
			mPools[AllocIndex].Free(Ptr);
			return;
		}
		if (FreedHeader->mNext != InvalidIndex) {
			header* NextHeader = GetHeader(FreedHeader->mNext, FreedHeader->mAllocIndex);
			if (!NextHeader->mOccupied) {
				mFreeHeaders.Remove(free_header_data{NextHeader, NextHeader->mSize});
				mNumBlocks--;
				FreedHeader->mSize += NextHeader->mSize + HeaderSize;
				FreedHeader->mNext = NextHeader->mNext;
				if (FreedHeader->mNext != InvalidIndex) {
					header* NewNextHeader = GetHeader(FreedHeader->mNext, FreedHeader->mAllocIndex);
					u32 FreedHeaderIndex = GetIndex(FreedHeader);
					NewNextHeader->mPrev = FreedHeaderIndex;
				}
			}
		}
		if (FreedHeader->mPrev != InvalidIndex) {
			header* PrevHeader = GetHeader(FreedHeader->mPrev, FreedHeader->mAllocIndex);
			if (!PrevHeader->mOccupied) {
				u32 PrevHeaderIndex = FreedHeader->mPrev;
				mFreeHeaders.Remove(free_header_data{PrevHeader, PrevHeader->mSize});
				mNumBlocks--;
				PrevHeader->mSize += FreedHeader->mSize + HeaderSize;
				PrevHeader->mNext = FreedHeader->mNext;
				if (PrevHeader->mNext != InvalidIndex) {
					header* NextHeader = GetHeader(PrevHeader->mNext, PrevHeader->mAllocIndex);
					NextHeader->mPrev = PrevHeaderIndex;
				}
				FreedHeader = PrevHeader;
			}
		}
		FreedHeader->mOccupied = false;
		mFreeHeaders.Add(free_header_data{FreedHeader, FreedHeader->mSize});
	}
};

void tree_allocator::ClearStaticImpl() {
	tree_allocator_impl::Impl.Clear();
}

void* tree_allocator::StaticAllocateImpl(u64 Size, u8 Alignment) {
	return tree_allocator_impl::Impl.Allocate(tree_allocator_impl::GranularSize(Size));
}

void tree_allocator::StaticFreeImpl(void* Ptr) {
	tree_allocator_impl::Impl.Free(Ptr);
}

bool tree_allocator::StaticExpandImpl(void* Ptr, u64 NewSize) {
	return tree_allocator_impl::Impl.Expand(Ptr, tree_allocator_impl::GranularSize(NewSize));
}

static_assert(sizeof(tree_allocator_impl::header) == tree_allocator_impl::Granularity);
static_assert(
	sizeof(tree_allocator_impl::header) - offsetof(tree_allocator_impl::header, mAllocIndex) ==
	sizeof(tree_allocator_impl::pool::pool_header) -
		offsetof(tree_allocator_impl::pool::pool_header, mAllocIndex));
tree_allocator_impl tree_allocator_impl::Impl{};
