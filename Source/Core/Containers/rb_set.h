#pragma once

#include "Templates/equals.h"
#include "Templates/less.h"
#include "Memory/memory.h"

template <typename tree_type, bool Const>
struct tree_iter {
	using value_type = tree_type::value_type;
	using pointer = std::conditional<Const, const value_type*, value_type*>::type;
	using reference = std::conditional<Const, const value_type&, value_type&>::type;
	using node_ptr = tree_type::tree_node*;

	node_ptr mNode = nullptr;

	FORCEINLINE tree_iter() = default;
	FORCEINLINE tree_iter(const tree_iter&) = default;
	FORCEINLINE tree_iter(tree_iter&&) noexcept = default;
	FORCEINLINE ~tree_iter() = default;

	FORCEINLINE explicit tree_iter(node_ptr Root, bool Reversed = false) : mNode(Root) {
		if (mNode) {
			if (!Reversed) {
				while (mNode->mLeftChild) {
					mNode = mNode->mLeftChild;
				}
			} else {
				while (mNode->mRightChild) {
					mNode = mNode->mRightChild;
				}
			}
		}
	}

	FORCEINLINE tree_iter& operator++() {
		if (mNode->mRightChild) {
			mNode = mNode->mRightChild;
			while (mNode->mLeftChild) {
				mNode = mNode->mLeftChild;
			}
		} else if (mNode->mParent) {
			bool RightChild;
			do {
				RightChild = mNode->mParent && mNode == mNode->mParent->mRightChild;
				mNode = mNode->mParent;
			} while (RightChild);
		} else {
			mNode = nullptr;
		}
		return *this;
	}

	FORCEINLINE tree_iter& operator--() {
		if (mNode->mLeftChild) {
			mNode = mNode->mLeftChild;
			while (mNode->mRightChild) {
				mNode = mNode->mRightChild;
			}
		} else if (mNode->mParent) {
			bool LeftChild;
			do {
				LeftChild = mNode->mParent && mNode == mNode->mParent->mLeftChild;
				mNode = mNode->mParent;
			} while (LeftChild);
		} else {
			mNode = nullptr;
		}
		return *this;
	}

	FORCEINLINE pointer operator->() {
		return &(mNode->mValue);
	}

	FORCEINLINE reference operator*() {
		return (mNode->mValue);
	}

	FORCEINLINE bool operator==(const tree_iter& Other) const {
		return mNode == Other.mNode;
	}
};

template <
	typename element_type,
	typename less_op = default_less_op,
	typename equals_op = default_equals_op,
	typename allocator_type = default_allocator>
struct rb_set : allocator_instance<allocator_type>, trait_memcopy_relocatable {
	enum class direction : u8 { left, right };
	enum class color : u8 { black, red };

	FORCEINLINE static constexpr direction Opposite(direction Direction) {
		return Direction == direction::left ? direction::right : direction::left;
	}

	using value_type = element_type;
	using iter = tree_iter<rb_set, false>;
	using const_iter = tree_iter<rb_set, true>;
	using alloc_base = allocator_instance<allocator_type>;

	index mSize = 0;
	struct tree_node;
	tree_node* mRoot = nullptr;

	struct tree_node {
		tree_node() = delete;

		explicit tree_node(const element_type& Elem) : mValue(Elem) {
		}

		element_type mValue;
		tree_node* mParent = nullptr;
		tree_node* mLeftChild = nullptr;
		tree_node* mRightChild = nullptr;
		color mColor = color::black;

		FORCEINLINE tree_node* GetSibling() {
			CHECK(mParent);
			if (mParent->mRightChild == this) {
				return mParent->mLeftChild;
			} else {
				return mParent->mRightChild;
			}
		}

		FORCEINLINE tree_node* GetUncle() {
			CHECK(mParent);
			return mParent->GetSibling();
		}

		FORCEINLINE tree_node* GetGrandparent() {
			CHECK(mParent);
			return mParent->mParent;
		}

		FORCEINLINE tree_node*& GetParentSlot() {
			CHECK(mParent);
			return mParent->mLeftChild == this ? mParent->mLeftChild : mParent->mRightChild;
		}

		FORCEINLINE constexpr direction GetDirection() {
			CHECK(mParent)
			return mParent->mLeftChild == this ? direction::left : direction::right;
		}

		FORCEINLINE constexpr tree_node*& GetChild(direction Direction) {
			return Direction == direction::left ? mLeftChild : mRightChild;
		}

		FORCEINLINE void SwapPlaces(rb_set* Set, tree_node* OtherNode) {
			if (this == OtherNode) {
				return;
			}

			const bool ThisIsParentOfOther = OtherNode->mParent == this;
			const bool OtherIsParentOfThis = mParent == OtherNode;

			const auto ThisParent = mParent != OtherNode ? mParent : this;
			const auto ThisLeftChild = mLeftChild != OtherNode ? mLeftChild : this;
			const auto ThisRightChild = mRightChild != OtherNode ? mRightChild : this;
			const auto OtherParent = OtherNode->mParent != this ? OtherNode->mParent : OtherNode;
			const auto OtherLeftChild = OtherNode->mLeftChild != this ? OtherNode->mLeftChild : OtherNode;
			const auto OtherRightChild = OtherNode->mRightChild != this ? OtherNode->mRightChild : OtherNode;

			// fix parent pointers to swapped nodes
			if (OtherNode->mParent) {
				if (ThisIsParentOfOther) {
					OtherNode->GetParentSlot() = OtherNode->GetChild(OtherNode->GetDirection());
				} else {
					OtherNode->GetParentSlot() = this;
				}
			} else {
				Set->mRoot = this;
			}
			if (mParent) {
				if (OtherIsParentOfThis) {
					GetParentSlot() = GetChild(GetDirection());
				} else {
					GetParentSlot() = OtherNode;
				}
			} else {
				Set->mRoot = OtherNode;
			}

			// fix children pointers to swapped nodes
			if (OtherNode->mLeftChild) {
				if (OtherNode->mLeftChild == this) {
					OtherNode->mLeftChild->mParent = OtherNode->mParent;
				} else {
					OtherNode->mLeftChild->mParent = this;
				}
			}
			if (OtherNode->mRightChild) {
				if (OtherNode->mRightChild == this) {
					OtherNode->mRightChild->mParent = OtherNode->mParent;
				} else {
					OtherNode->mRightChild->mParent = this;
				}
			}
			if (mLeftChild) {
				if (mLeftChild == OtherNode) {
					mLeftChild->mParent = mParent;
				} else {
					mLeftChild->mParent = OtherNode;
				}
			}
			if (mRightChild) {
				if (mRightChild == OtherNode) {
					mRightChild->mParent = mParent;
				} else {
					mRightChild->mParent = OtherNode;
				};
			}

			const auto Color = mColor;
			mColor = OtherNode->mColor;
			OtherNode->mColor = Color;
			mParent = OtherParent;
			OtherNode->mParent = ThisParent;
			mLeftChild = OtherLeftChild;
			OtherNode->mLeftChild = ThisLeftChild;
			mRightChild = OtherRightChild;
			OtherNode->mRightChild = ThisRightChild;
		}
	};

	rb_set() = default;

	~rb_set() {
		Clear();
	}

	FORCEINLINE element_type* AddUnique(const element_type& Elem) {
		tree_node* Parent = nullptr;
		tree_node** CurrentSlot = &mRoot;
		for (;;) {
			if (*CurrentSlot == nullptr) {
				auto* NewNode = new (alloc_base::mAllocator.Allocate(sizeof(tree_node))) tree_node(Elem);
				*CurrentSlot = NewNode;
				if (Parent) {
					NewNode->mParent = Parent;
					NewNode->mColor = color::red;
					FixAddition(NewNode);
				}
				++mSize;
				return &(NewNode->mValue);
			} else {
				Parent = *CurrentSlot;
				if (less_op::Less(Elem, Parent->mValue)) {
					CurrentSlot = &(Parent->mLeftChild);
				} else if (equals_op::Equals(Elem, Parent->mValue)) {
					return nullptr;
				} else {
					CurrentSlot = &(Parent->mRightChild);
				}
			}
		}
	}

	FORCEINLINE element_type* Add(const element_type& Elem) {
		tree_node* Parent = nullptr;
		tree_node** CurrentSlot = &mRoot;
		for (;;) {
			if (*CurrentSlot == nullptr) {
				auto* NewNode = new (alloc_base::mAllocator.Allocate(sizeof(tree_node))) tree_node(Elem);
				*CurrentSlot = NewNode;
				if (Parent) {
					NewNode->mParent = Parent;
					NewNode->mColor = color::red;
					FixAddition(NewNode);
				}
				++mSize;
				return &(NewNode->mValue);
			} else {
				Parent = *CurrentSlot;
				if (less_op::Less(Elem, Parent->mValue)) {
					CurrentSlot = &(Parent->mLeftChild);
				} else {
					CurrentSlot = &(Parent->mRightChild);
				}
			}
		}
	}

	FORCEINLINE void FixAddition(tree_node* AddedNode) {
		for (;;) {
			auto* Parent = AddedNode->mParent;
			if (!Parent || Parent->mColor == color::black) {
				return;
			}
			auto* Grandparent = AddedNode->GetGrandparent();
			if (!Grandparent) {
				Parent->mColor = color::black;
				return;
			}
			auto* Uncle = AddedNode->GetUncle();
			if (Uncle && Uncle->mColor == color::red) {
				Uncle->mColor = color::black;
				Parent->mColor = color::black;
				Uncle->mParent->mColor = color::red;
				AddedNode = Uncle->mParent;
				continue;
			}
			direction OuterDirection = Parent->GetDirection();
			if (AddedNode->GetDirection() != OuterDirection) {
				Rotate(Parent, OuterDirection);
				Parent = Grandparent->GetChild(OuterDirection);
			}
			Rotate(Grandparent, Opposite(OuterDirection));
			Parent->mColor = color::black;
			Grandparent->mColor = color::red;
			return;
		}
	}

	FORCEINLINE void Rotate(tree_node* RotationRoot, direction Direction) {
		if (Direction == direction::right) {
			Rotate<direction::right>(RotationRoot);
		} else {
			Rotate<direction::left>(RotationRoot);
		}
	}

	template <direction Direction>
	FORCEINLINE void Rotate(tree_node* RotationRoot) {
		tree_node* NewRoot = RotationRoot->GetChild(Opposite(Direction));
		CHECK(NewRoot);
		tree_node* Parent = RotationRoot->mParent;
		tree_node* MiddleChild = NewRoot->GetChild(Direction);
		RotationRoot->GetChild(Opposite(Direction)) = MiddleChild;
		if (MiddleChild) {
			MiddleChild->mParent = RotationRoot;
		}
		NewRoot->GetChild(Direction) = RotationRoot;
		NewRoot->mParent = Parent;
		if (Parent) {
			RotationRoot->GetParentSlot() = NewRoot;
		} else {
			mRoot = NewRoot;
		}
		RotationRoot->mParent = NewRoot;
	}

	FORCEINLINE bool Remove(const element_type& Elem) {
		tree_node* NodeToDelete = mRoot;
		while (NodeToDelete) {
			if (equals_op::Equals(NodeToDelete->mValue, Elem)) {
				FixRemoval(NodeToDelete);
				NodeToDelete->~tree_node();
				alloc_base::mAllocator.Free(NodeToDelete);
				--mSize;
				return true;
			}
			if (less_op::Less(Elem, NodeToDelete->mValue)) {
				NodeToDelete = NodeToDelete->mLeftChild;
			} else {
				NodeToDelete = NodeToDelete->mRightChild;
			}
		}
		return false;
	}
	
	FORCEINLINE bool RemoveByPtr(element_type* Elem) {
		tree_node* NodeToDelete = (tree_node*)Elem;
		FixRemoval(NodeToDelete);
		NodeToDelete->~tree_node();
		alloc_base::mAllocator.Free(NodeToDelete);
		--mSize;
		return true;
	}

	FORCEINLINE void FixRemoval(tree_node* NodeToDelete) {
		bool HasLeftChild = NodeToDelete->mLeftChild;
		bool HasRightChild = NodeToDelete->mRightChild;
		const bool HasNoChildren = !HasLeftChild && !HasRightChild;
		if (!NodeToDelete->mParent && HasNoChildren) {
			mRoot = nullptr;
			return;
		}
		const bool HasBothChildren = HasLeftChild && HasRightChild;
		if (HasBothChildren) {
			auto* SuccessorNode = NodeToDelete->mRightChild;
			while (SuccessorNode->mLeftChild) {
				SuccessorNode = SuccessorNode->mLeftChild;
			}
			NodeToDelete->SwapPlaces(this, SuccessorNode);
			HasLeftChild = NodeToDelete->mLeftChild;
			HasRightChild = NodeToDelete->mRightChild;
		}
		if (NodeToDelete->mColor == color::red) {
			// guaranteed no children
			if (NodeToDelete->mParent) {
				NodeToDelete->GetParentSlot() = nullptr;
			} else {
				mRoot = nullptr;
			}
			return;
		}
		const bool HasOneChild = HasLeftChild != HasRightChild;
		if (HasOneChild) {
			// guaranteed red child on black node
			auto* Child = HasLeftChild ? NodeToDelete->mLeftChild : NodeToDelete->mRightChild;
			Child->mColor = color::black;
			if (NodeToDelete->mParent) {
				Child->mParent = NodeToDelete->mParent;
				NodeToDelete->GetParentSlot() = Child;
			} else {
				Child->mParent = nullptr;
				mRoot = Child;
			}
			return;
		}
		// complex case: no children, black non-root node
		tree_node* InvalidNode = NodeToDelete;
		tree_node* Parent;
		tree_node* Sibling;
		tree_node* CloseNephew;
		tree_node* DistantNephew;
		direction NodeDirection;
		bool BlackParent;
		bool BlackSibling;
		bool BlackCloseNephew;
		bool BlackDistantNephew;
		for (;;) {
			if (!InvalidNode->mParent) {
				if (NodeToDelete->mParent) {
					NodeToDelete->GetParentSlot() = nullptr;
				} else {
					mRoot = nullptr;
				}
				return;
			}
			NodeDirection = InvalidNode->GetDirection();
			// guaranteed sibling, because black height on our side is not 0
			Parent = InvalidNode->mParent;
			Sibling = InvalidNode->GetSibling();
			CloseNephew = Sibling->GetChild(NodeDirection);
			DistantNephew = Sibling->GetChild(Opposite(NodeDirection));
			BlackParent = Parent->mColor == color::black;
			BlackSibling = Sibling->mColor == color::black;
			BlackCloseNephew = !CloseNephew || CloseNephew->mColor == color::black;
			BlackDistantNephew = !DistantNephew || DistantNephew->mColor == color::black;
			if (BlackParent && BlackSibling && BlackCloseNephew && BlackDistantNephew) {
				Sibling->mColor = color::red;
				InvalidNode = InvalidNode->mParent;
			} else {
				break;
			}
		}
		if (!BlackSibling) {
			Rotate(Parent, NodeDirection);
			Parent->mColor = color::red;
			BlackParent = false;
			Sibling->mColor = color::black;
			Sibling = CloseNephew;
			BlackSibling = BlackCloseNephew;
			CloseNephew = Sibling->GetChild(NodeDirection);
			DistantNephew = Sibling->GetChild(Opposite(NodeDirection));
			BlackCloseNephew = !CloseNephew || CloseNephew->mColor == color::black;
			BlackDistantNephew = !DistantNephew || DistantNephew->mColor == color::black;
		}
		if (!BlackParent && BlackCloseNephew && BlackDistantNephew) {
			Sibling->mColor = color::red;
			Parent->mColor = color::black;
			if (NodeToDelete->mParent) {
				NodeToDelete->GetParentSlot() = nullptr;
			} else {
				mRoot = nullptr;
			}
			return;
		}
		if (BlackSibling && !BlackCloseNephew && BlackDistantNephew) {
			Rotate(Sibling, Opposite(NodeDirection));
			Sibling->mColor = color::red;
			CloseNephew->mColor = color::black;
			DistantNephew = Sibling;
			BlackDistantNephew = false;
			Sibling = CloseNephew;
			BlackSibling = true;
			CloseNephew = Sibling->GetChild(NodeDirection);
			BlackCloseNephew = !CloseNephew || CloseNephew->mColor == color::black;
		}
		if (BlackSibling && !BlackDistantNephew) {
			Rotate(Parent, NodeDirection);
			Sibling->mColor = Parent->mColor;
			Parent->mColor = color::black;
			DistantNephew->mColor = color::black;
			if (NodeToDelete->mParent) {
				NodeToDelete->GetParentSlot() = nullptr;
			} else {
				mRoot = nullptr;
			}
			return;
		}
	}

	FORCEINLINE bool Contains(const element_type& Elem) const {
		return Find(Elem);
	}

	FORCEINLINE element_type* Find(const element_type& Elem) const {
		tree_node* Current = mRoot;
		while (Current) {
			if (equals_op::Equals(Current->mValue, Elem)) {
				return &(Current->mValue);
			}
			if (less_op::Less(Elem, Current->mValue)) {
				Current = Current->mLeftChild;
			} else {
				Current = Current->mRightChild;
			}
		}
		return nullptr;
	}

	FORCEINLINE element_type* LowerBound(const element_type& Elem) {
		tree_node* Current = mRoot;
		tree_node* Result = nullptr;
		while (Current) {
			if (equals_op::Equals(Current->mValue, Elem)) {
				return &(Current->mValue);
			}
			if (less_op::Less(Elem, Current->mValue)) {
				Current = Current->mLeftChild;
			} else {
				Result =
					Result && !less_op::Less(Result->mValue, Current->mValue) ? Result : Current;
				Current = Current->mRightChild;
			}
		}
		return Result ? &(Result->mValue) : nullptr;
	}

	FORCEINLINE element_type* UpperBound(const element_type& Elem) {
		tree_node* Current = mRoot;
		tree_node* Result = nullptr;
		while (Current) {
			if (equals_op::Equals(Current->mValue, Elem)) {
				return &(Current->mValue);
			}
			if (less_op::Less(Current->mValue, Elem)) {
				Current = Current->mRightChild;
			} else {
				Result =
					Result && less_op::Less(Result->mValue, Current->mValue) ? Result : Current;
				Current = Current->mLeftChild;
			}
		}
		return Result ? &(Result->mValue) : nullptr;
	}

	FORCEINLINE iter begin() {
		return iter(mRoot);
	}

	FORCEINLINE iter rbegin() {
		return iter(mRoot, true);
	}

	FORCEINLINE iter end() {
		return iter(nullptr);
	}

	FORCEINLINE iter rend() {
		return iter(nullptr);
	}

	FORCEINLINE const_iter begin() const {
		return const_iter(mRoot);
	}

	FORCEINLINE const_iter rbegin() const {
		return const_iter(mRoot, true);
	}

	FORCEINLINE const_iter end() const {
		return const_iter(nullptr);
	}

	FORCEINLINE const_iter rend() const {
		return const_iter(nullptr);
	}

	void Clear() {
		tree_node* Stack[32];
		// slower but more general variant if tree is balanced
		// index_type MaxDepth = LogOfTwoCeil(mSize) + 1;
		// auto** Stack = (tree_node**)alloca(MaxDepth * sizeof(tree_node*));
		Stack[0] = mRoot;
		for (tree_node** StackTop = Stack; StackTop >= Stack; --StackTop) {
			tree_node* Current = *StackTop;
			while (Current) {
				tree_node* NodeToDelete = Current;
				if (NodeToDelete->mLeftChild) {
					if (NodeToDelete->mRightChild) {
						*StackTop = NodeToDelete->mRightChild;
						++StackTop;
					}
					Current = NodeToDelete->mLeftChild;
				} else {
					Current = NodeToDelete->mRightChild;
				}
				NodeToDelete->~tree_node();
				alloc_base::mAllocator.Free(NodeToDelete);
			}
		}
		mRoot = nullptr;
		mSize = 0;
	}
};

// TODO: not needed for anything right now
// template <
//	typename table_key_type,
//	typename table_value_type,
//	typename less_op = default_less_op,
//	typename equals_op = default_equals_op,
//	typename allocator = default_allocator>
// struct rb_table
//	: public rb_set<key_value_pair<table_key_type, table_value_type>, less_op, equals_op, allocator>
//{ 	using table_pair = key_value_pair<table_key_type, table_value_type>; 	using super =
// rb_set<table_pair, less_op, equals_op, allocator > ;
//
//	FORCEINLINE explicit rb_table() = default;
//
//	FORCEINLINE table_pair* Add(const table_key_type& Key, const table_value_type& Value) {
//	}
//
//	FORCEINLINE bool Remove(const table_key_type& Key) {
//	}
//
//	[[nodiscard]] FORCEINLINE table_value_type& operator[](const table_key_type& Key) {
//	}
//
//	FORCEINLINE bool Contains(const table_key_type& Key) const {
//		return Find(Key);
//	}
//
//	FORCEINLINE table_value_type* Find(const table_key_type& Key) {
//	}
//
//	FORCEINLINE table_pair* LowerBound(const table_key_type& Key) {
//	}
//
//	FORCEINLINE table_pair* UpperBound(const table_key_type& Key) {
//	}
//};
