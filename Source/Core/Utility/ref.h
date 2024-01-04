#pragma once

#include "basic.h"
#include "Memory/memory.h"

template <typename ref_counted_type>
concept ref_counted = requires(ref_counted_type instance) {
	instance.RefCounter;
	instance.Reference();
	instance.Dereference();
};

template <ref_counted ref_counted_type>
struct ref {
private:
	ref_counted_type* Pointer = nullptr;

public:
	FORCEINLINE ref() = default;

	FORCEINLINE ~ref() {
		Unref();
	}

	void Unref() {
		if (Pointer) {
			if (!Pointer->Dereference()) {
				// pointed instance should be allocated with default_allocator
				// NOTE: maybe this should be somehow enforced?
				StaticDelete(Pointer);
			}
			Pointer = nullptr;
		}
	}

	template <typename... arg_types>
	void Create(arg_types&&... Args) {
		ref(StaticNew<ref_counted_type>(std::forward<arg_types>(Args)...));
	}

	FORCEINLINE explicit ref(ref_counted_type* InPointer) {
		if (InPointer == Pointer) {
			return;
		}
		Unref();
		if (InPointer) {
			InPointer->Reference();
			Pointer = InPointer;
		}
	}

	FORCEINLINE ref(const ref& Other) : ref(Other.Pointer) {
	}

	FORCEINLINE ref(ref&& Other) : Pointer{Other.Pointer} {
		Other.Pointer = nullptr;
	}

	FORCEINLINE bool operator==(const ref_counted_type* InPointer) const {
		return Pointer == InPointer;
	}

	FORCEINLINE bool operator!=(const ref_counted_type* InPointer) const {
		return Pointer != InPointer;
	}

	FORCEINLINE bool operator==(const ref& Other) const {
		return Pointer == Other.Pointer;
	}

	FORCEINLINE bool operator!=(const ref& Other) const {
		return Pointer != Other.Pointer;
	}

	const ref_counted_type* operator->() const {
		return Pointer;
	}

	ref_counted_type* operator->() {
		return Pointer;
	}

	ref_counted_type& operator*() noexcept {
		return *Pointer;
	}

	const ref_counted_type& operator*() const {
		return *Pointer;
	}

	FORCEINLINE ref& operator=(const ref& Other) {
		ref(Other.Pointer);
	}

	FORCEINLINE ref& operator=(ref&& Other) {
		Pointer = Other.Pointer;
		Other.Pointer = nullptr;
	}

	FORCEINLINE bool Valid() const {
		return Pointer != nullptr;
	}
};

struct ref_counted_base {
private:
	u32 RefCounter{0};

public:
	void Reference() {
		++RefCounter;
	}

	bool Dereference() {
		return --RefCounter;
	}
};
