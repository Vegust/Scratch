#pragma once

#include "basic.h"
#include "span.h"
#include "Templates/concepts.h"
#include "Templates/less.h"

namespace algo {

// NOTE: ideally I would want this to only work with spans, but implicit conversions to span are tricky

template <typename container_type>
FORCEINLINE index FindFirst(const container_type& Container, const typename container_type::value_type& Value) {
	for (index Index = 0; Index < Container.GetSize(); ++Index) {
		if (Value == Container[Index]) {
			return Index;
		}
	}
	return InvalidIndex;
}

template <typename container_type>
FORCEINLINE index FindLast(const container_type& Container, const typename container_type::value_type& Value) {
	for (index Index = Container.GetSize() - 1;; --Index) {
		if (Value == Container[Index]) {
			return Index;
		}
		if (Index == 0) {
			break;
		}
	}
	return InvalidIndex;
}

template <typename container_type, typename predicate_type>
FORCEINLINE index FindFirstByPredicate(const container_type& Container, const predicate_type& Predicate) {
	for (index Index = 0; Index < Container.GetSize(); ++Index) {
		if (Predicate(Container[Index])) {
			return Index;
		}
	}
	return InvalidIndex;
}

template <typename container_type, typename predicate_type>
FORCEINLINE index FindLastByPredicate(const container_type& Container, const predicate_type& Predicate) {
	for (index Index = Container.GetSize() - 1;; --Index) {
		if (Predicate(Container[Index])) {
			return Index;
		}
		if (Index == 0) {
			break;
		}
	}
	return InvalidIndex;
}

template <typename element_type>
FORCEINLINE void SwapElements(element_type& Rhs, element_type& Lhs) {
	if constexpr (memcopy_relocatable<element_type>) {
		Memswap(Rhs, Lhs);
	} else {
		std::swap(Rhs, Lhs);
	}
}

template <typename element_type, typename less_type>
FORCEINLINE void InsertionSort(element_type* Begin, element_type* End, less_type LessOp) {
	if (Begin == End) {
		return;
	}
	for (element_type* Mid = Begin + 1; Mid != End; ++Mid) {
		if constexpr (memcopy_relocatable<element_type>) {
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

template <typename element_type, typename less_type>
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

template <typename element_type, typename less_type>
void MergeSort(element_type* Begin, element_type* End, less_type LessOp) {
	// TODO
}

template <typename container_type, typename less_type = default_less_op>
FORCEINLINE void Sort(container_type& Container, less_type LessOp = default_less_op{}) {
	Quicksort<typename container_type::value_type, less_type>(Container.begin(), Container.end(), LessOp);
}

template <typename container_type, typename less_type = default_less_op>
FORCEINLINE void StableSort(container_type& Container, less_type LessOp = default_less_op{}) {
	MergeSort<typename container_type::value_type, less_type>(Container.begin(), Container.end(), LessOp);
}

}	 // namespace algo