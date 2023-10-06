#pragma once

#include "core_types.h"
#include "Memory/memory.h"

template <typename ref_counted_type>
concept ref_counted_c = requires(ref_counted_type instance) {
	instance.mRefCounter;
	instance.Reference();
	instance.Dereference();
};

template <ref_counted_c ref_counted_type>
struct ref {
	ref_counted_type* mPointer = nullptr;

	FORCEINLINE ref() = default;

	FORCEINLINE ~ref() {
		Unref();
	}

	void Unref() {
		if (mPointer) {
			if (!mPointer->Dereference()) {
				StaticDelete(mPointer);
			}
			mPointer = nullptr;
		}
	}

	template <typename... arg_types>
	void Create(arg_types&&... Args) {
		ref(StaticNew<ref_counted_type>(std::forward<arg_types>(Args)...));
	}

	// pointed instance should be allocated with default_allocator
	FORCEINLINE ref(ref_counted_type* Pointer) {
		if (Pointer == mPointer) {
			return;
		}
		Unref();
		if (Pointer) {
			Pointer->Reference();
			mPointer = Pointer;
		}
	}

	FORCEINLINE ref(const ref& Other) : ref(Other.mPointer) {
	}

	FORCEINLINE bool operator==(const ref_counted_type* Pointer) const {
		return mPointer == Pointer;
	}

	FORCEINLINE bool operator!=(const ref_counted_type* Pointer) const {
		return mPointer != Pointer;
	}

	FORCEINLINE bool operator==(const ref& Other) const {
		return mPointer == Other.mPointer;
	}

	FORCEINLINE bool operator!=(const ref& Other) const {
		return mPointer != Other.mPointer;
	}

	FORCEINLINE ref_counted_type& operator->() const {
		return *mPointer;
	}

	FORCEINLINE void operator=(const ref& Other) {
		ref(Other.mPointer);
	}

	FORCEINLINE bool Valid() const {
		return mPointer != nullptr;
	}
};

struct ref_counted {
	u32 mRefCounter{0};

	void Reference() {
		++mRefCounter;
	}

	bool Dereference() {
		return --mRefCounter;
	}
};
