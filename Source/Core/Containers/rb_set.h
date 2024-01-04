#pragma once

#include "Templates/equals.h"
#include "Templates/less.h"
#include "Memory/memory.h"

template <typename tree_type, iterator_constness Constness>
struct tree_iter {
	using value_type = tree_type::value_type;
	using pointer = std::conditional<Constness == iterator_constness::constant, const value_type*, value_type*>::type;
	using reference = std::conditional<Constness == iterator_constness::constant, const value_type&, value_type&>::type;
	using node_ptr = tree_type::tree_node*;

	node_ptr Node = nullptr;

	enum class iter_direction { forward, reversed };

	FORCEINLINE tree_iter() = default;
	FORCEINLINE tree_iter(const tree_iter&) = default;
	FORCEINLINE tree_iter(tree_iter&&) noexcept = default;
	FORCEINLINE ~tree_iter() = default;

	FORCEINLINE explicit tree_iter(node_ptr Root, iter_direction Direction = iter_direction::forward) : Node(Root) {
		if (Node) {
			if (Direction == iter_direction::forward) {
				while (Node->LeftChild) {
					Node = Node->LeftChild;
				}
			} else {
				while (Node->RightChild) {
					Node = Node->RightChild;
				}
			}
		}
	}

	FORCEINLINE tree_iter& operator++() {
		if (Node->RightChild) {
			Node = Node->RightChild;
			while (Node->LeftChild) {
				Node = Node->LeftChild;
			}
		} else if (Node->Parent) {
			bool RightChild;
			do {
				RightChild = Node->Parent && Node == Node->Parent->RightChild;
				Node = Node->Parent;
			} while (RightChild);
		} else {
			Node = nullptr;
		}
		return *this;
	}

	FORCEINLINE tree_iter& operator--() {
		if (Node->LeftChild) {
			Node = Node->LeftChild;
			while (Node->RightChild) {
				Node = Node->RightChild;
			}
		} else if (Node->Parent) {
			bool LeftChild;
			do {
				LeftChild = Node->Parent && Node == Node->Parent->LeftChild;
				Node = Node->Parent;
			} while (LeftChild);
		} else {
			Node = nullptr;
		}
		return *this;
	}

	FORCEINLINE pointer operator->() {
		return &(Node->Value);
	}

	FORCEINLINE reference operator*() {
		return (Node->Value);
	}

	FORCEINLINE bool operator==(const tree_iter& Other) const {
		return Node == Other.Node;
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
	using iter = tree_iter<rb_set, iterator_constness::non_constant>;
	using const_iter = tree_iter<rb_set, iterator_constness::constant>;
	using alloc_base = allocator_instance<allocator_type>;

	index Size = 0;
	struct tree_node;
	tree_node* Root = nullptr;

	struct tree_node {
		tree_node() = delete;

		explicit tree_node(const element_type& Elem) : Value(Elem) {
		}

		element_type Value;
		tree_node* Parent = nullptr;
		tree_node* LeftChild = nullptr;
		tree_node* RightChild = nullptr;
		color Color = color::black;

		FORCEINLINE tree_node* GetSibling() {
			CHECK(Parent);
			if (Parent->RightChild == this) {
				return Parent->LeftChild;
			} else {
				return Parent->RightChild;
			}
		}

		FORCEINLINE tree_node* GetUncle() {
			CHECK(Parent);
			return Parent->GetSibling();
		}

		FORCEINLINE tree_node* GetGrandparent() {
			CHECK(Parent);
			return Parent->Parent;
		}

		FORCEINLINE tree_node*& GetParentSlot() {
			CHECK(Parent);
			return Parent->LeftChild == this ? Parent->LeftChild : Parent->RightChild;
		}

		FORCEINLINE constexpr direction GetDirection() {
			CHECK(Parent)
			return Parent->LeftChild == this ? direction::left : direction::right;
		}

		FORCEINLINE constexpr tree_node*& GetChild(direction Direction) {
			return Direction == direction::left ? LeftChild : RightChild;
		}

		FORCEINLINE void SwapPlaces(rb_set* Set, tree_node* OtherNode) {
			if (this == OtherNode) {
				return;
			}

			const bool ThisIsParentOfOther = OtherNode->Parent == this;
			const bool OtherIsParentOfThis = Parent == OtherNode;

			const auto ThisParent = Parent != OtherNode ? Parent : this;
			const auto ThisLeftChild = LeftChild != OtherNode ? LeftChild : this;
			const auto ThisRightChild = RightChild != OtherNode ? RightChild : this;
			const auto OtherParent = OtherNode->Parent != this ? OtherNode->Parent : OtherNode;
			const auto OtherLeftChild = OtherNode->LeftChild != this ? OtherNode->LeftChild : OtherNode;
			const auto OtherRightChild = OtherNode->RightChild != this ? OtherNode->RightChild : OtherNode;

			// fix parent pointers to swapped nodes
			if (OtherNode->Parent) {
				if (ThisIsParentOfOther) {
					OtherNode->GetParentSlot() = OtherNode->GetChild(OtherNode->GetDirection());
				} else {
					OtherNode->GetParentSlot() = this;
				}
			} else {
				Set->Root = this;
			}
			if (Parent) {
				if (OtherIsParentOfThis) {
					GetParentSlot() = GetChild(GetDirection());
				} else {
					GetParentSlot() = OtherNode;
				}
			} else {
				Set->Root = OtherNode;
			}

			// fix children pointers to swapped nodes
			if (OtherNode->LeftChild) {
				if (OtherNode->LeftChild == this) {
					OtherNode->LeftChild->Parent = OtherNode->Parent;
				} else {
					OtherNode->LeftChild->Parent = this;
				}
			}
			if (OtherNode->RightChild) {
				if (OtherNode->RightChild == this) {
					OtherNode->RightChild->Parent = OtherNode->Parent;
				} else {
					OtherNode->RightChild->Parent = this;
				}
			}
			if (LeftChild) {
				if (LeftChild == OtherNode) {
					LeftChild->Parent = Parent;
				} else {
					LeftChild->Parent = OtherNode;
				}
			}
			if (RightChild) {
				if (RightChild == OtherNode) {
					RightChild->Parent = Parent;
				} else {
					RightChild->Parent = OtherNode;
				};
			}

			const auto SwappedColor = Color;
			Color = OtherNode->Color;
			OtherNode->Color = SwappedColor;
			Parent = OtherParent;
			OtherNode->Parent = ThisParent;
			LeftChild = OtherLeftChild;
			OtherNode->LeftChild = ThisLeftChild;
			RightChild = OtherRightChild;
			OtherNode->RightChild = ThisRightChild;
		}
	};

	rb_set() = default;

	~rb_set() {
		Clear();
	}

	FORCEINLINE element_type* AddUnique(const element_type& Elem) {
		tree_node* Parent = nullptr;
		tree_node** CurrentSlot = &Root;
		for (;;) {
			if (*CurrentSlot == nullptr) {
				auto* NewNode = new (alloc_base::Allocator.Allocate(sizeof(tree_node))) tree_node(Elem);
				*CurrentSlot = NewNode;
				if (Parent) {
					NewNode->Parent = Parent;
					NewNode->Color = color::red;
					FixAddition(NewNode);
				}
				++Size;
				return &(NewNode->Value);
			} else {
				Parent = *CurrentSlot;
				if (less_op::Less(Elem, Parent->Value)) {
					CurrentSlot = &(Parent->LeftChild);
				} else if (equals_op::Equals(Elem, Parent->Value)) {
					return nullptr;
				} else {
					CurrentSlot = &(Parent->RightChild);
				}
			}
		}
	}

	FORCEINLINE element_type* Add(const element_type& Elem) {
		tree_node* Parent = nullptr;
		tree_node** CurrentSlot = &Root;
		for (;;) {
			if (*CurrentSlot == nullptr) {
				auto* NewNode = new (alloc_base::Allocator.Allocate(sizeof(tree_node))) tree_node(Elem);
				*CurrentSlot = NewNode;
				if (Parent) {
					NewNode->Parent = Parent;
					NewNode->Color = color::red;
					FixAddition(NewNode);
				}
				++Size;
				return &(NewNode->Value);
			} else {
				Parent = *CurrentSlot;
				if (less_op::Less(Elem, Parent->Value)) {
					CurrentSlot = &(Parent->LeftChild);
				} else {
					CurrentSlot = &(Parent->RightChild);
				}
			}
		}
	}

	FORCEINLINE void FixAddition(tree_node* AddedNode) {
		for (;;) {
			auto* Parent = AddedNode->Parent;
			if (!Parent || Parent->Color == color::black) {
				return;
			}
			auto* Grandparent = AddedNode->GetGrandparent();
			if (!Grandparent) {
				Parent->Color = color::black;
				return;
			}
			auto* Uncle = AddedNode->GetUncle();
			if (Uncle && Uncle->Color == color::red) {
				Uncle->Color = color::black;
				Parent->Color = color::black;
				Uncle->Parent->Color = color::red;
				AddedNode = Uncle->Parent;
				continue;
			}
			direction OuterDirection = Parent->GetDirection();
			if (AddedNode->GetDirection() != OuterDirection) {
				Rotate(Parent, OuterDirection);
				Parent = Grandparent->GetChild(OuterDirection);
			}
			Rotate(Grandparent, Opposite(OuterDirection));
			Parent->Color = color::black;
			Grandparent->Color = color::red;
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
		tree_node* Parent = RotationRoot->Parent;
		tree_node* MiddleChild = NewRoot->GetChild(Direction);
		RotationRoot->GetChild(Opposite(Direction)) = MiddleChild;
		if (MiddleChild) {
			MiddleChild->Parent = RotationRoot;
		}
		NewRoot->GetChild(Direction) = RotationRoot;
		NewRoot->Parent = Parent;
		if (Parent) {
			RotationRoot->GetParentSlot() = NewRoot;
		} else {
			Root = NewRoot;
		}
		RotationRoot->Parent = NewRoot;
	}

	FORCEINLINE bool Remove(const element_type& Elem) {
		tree_node* NodeToDelete = Root;
		while (NodeToDelete) {
			if (equals_op::Equals(NodeToDelete->Value, Elem)) {
				FixRemoval(NodeToDelete);
				NodeToDelete->~tree_node();
				alloc_base::Allocator.Free(NodeToDelete);
				--Size;
				return true;
			}
			if (less_op::Less(Elem, NodeToDelete->Value)) {
				NodeToDelete = NodeToDelete->LeftChild;
			} else {
				NodeToDelete = NodeToDelete->RightChild;
			}
		}
		return false;
	}

	FORCEINLINE bool RemoveByPtr(element_type* Elem) {
		tree_node* NodeToDelete = (tree_node*) Elem;
		FixRemoval(NodeToDelete);
		NodeToDelete->~tree_node();
		alloc_base::Allocator.Free(NodeToDelete);
		--Size;
		return true;
	}

	FORCEINLINE void FixRemoval(tree_node* NodeToDelete) {
		bool HasLeftChild = NodeToDelete->LeftChild;
		bool HasRightChild = NodeToDelete->RightChild;
		const bool HasNoChildren = !HasLeftChild && !HasRightChild;
		if (!NodeToDelete->Parent && HasNoChildren) {
			Root = nullptr;
			return;
		}
		const bool HasBothChildren = HasLeftChild && HasRightChild;
		if (HasBothChildren) {
			auto* SuccessorNode = NodeToDelete->RightChild;
			while (SuccessorNode->LeftChild) {
				SuccessorNode = SuccessorNode->LeftChild;
			}
			NodeToDelete->SwapPlaces(this, SuccessorNode);
			HasLeftChild = NodeToDelete->LeftChild;
			HasRightChild = NodeToDelete->RightChild;
		}
		if (NodeToDelete->Color == color::red) {
			// guaranteed no children
			if (NodeToDelete->Parent) {
				NodeToDelete->GetParentSlot() = nullptr;
			} else {
				Root = nullptr;
			}
			return;
		}
		const bool HasOneChild = HasLeftChild != HasRightChild;
		if (HasOneChild) {
			// guaranteed red child on black node
			auto* Child = HasLeftChild ? NodeToDelete->LeftChild : NodeToDelete->RightChild;
			Child->Color = color::black;
			if (NodeToDelete->Parent) {
				Child->Parent = NodeToDelete->Parent;
				NodeToDelete->GetParentSlot() = Child;
			} else {
				Child->Parent = nullptr;
				Root = Child;
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
			if (!InvalidNode->Parent) {
				if (NodeToDelete->Parent) {
					NodeToDelete->GetParentSlot() = nullptr;
				} else {
					Root = nullptr;
				}
				return;
			}
			NodeDirection = InvalidNode->GetDirection();
			// guaranteed sibling, because black height on our side is not 0
			Parent = InvalidNode->Parent;
			Sibling = InvalidNode->GetSibling();
			CloseNephew = Sibling->GetChild(NodeDirection);
			DistantNephew = Sibling->GetChild(Opposite(NodeDirection));
			BlackParent = Parent->Color == color::black;
			BlackSibling = Sibling->Color == color::black;
			BlackCloseNephew = !CloseNephew || CloseNephew->Color == color::black;
			BlackDistantNephew = !DistantNephew || DistantNephew->Color == color::black;
			if (BlackParent && BlackSibling && BlackCloseNephew && BlackDistantNephew) {
				Sibling->Color = color::red;
				InvalidNode = InvalidNode->Parent;
			} else {
				break;
			}
		}
		if (!BlackSibling) {
			Rotate(Parent, NodeDirection);
			Parent->Color = color::red;
			BlackParent = false;
			Sibling->Color = color::black;
			Sibling = CloseNephew;
			BlackSibling = BlackCloseNephew;
			CloseNephew = Sibling->GetChild(NodeDirection);
			DistantNephew = Sibling->GetChild(Opposite(NodeDirection));
			BlackCloseNephew = !CloseNephew || CloseNephew->Color == color::black;
			BlackDistantNephew = !DistantNephew || DistantNephew->Color == color::black;
		}
		if (!BlackParent && BlackCloseNephew && BlackDistantNephew) {
			Sibling->Color = color::red;
			Parent->Color = color::black;
			if (NodeToDelete->Parent) {
				NodeToDelete->GetParentSlot() = nullptr;
			} else {
				Root = nullptr;
			}
			return;
		}
		if (BlackSibling && !BlackCloseNephew && BlackDistantNephew) {
			Rotate(Sibling, Opposite(NodeDirection));
			Sibling->Color = color::red;
			CloseNephew->Color = color::black;
			DistantNephew = Sibling;
			BlackDistantNephew = false;
			Sibling = CloseNephew;
			BlackSibling = true;
			CloseNephew = Sibling->GetChild(NodeDirection);
			BlackCloseNephew = !CloseNephew || CloseNephew->Color == color::black;
		}
		if (BlackSibling && !BlackDistantNephew) {
			Rotate(Parent, NodeDirection);
			Sibling->Color = Parent->Color;
			Parent->Color = color::black;
			DistantNephew->Color = color::black;
			if (NodeToDelete->Parent) {
				NodeToDelete->GetParentSlot() = nullptr;
			} else {
				Root = nullptr;
			}
			return;
		}
	}

	FORCEINLINE bool Contains(const element_type& Elem) const {
		return Find(Elem);
	}

	FORCEINLINE element_type* Find(const element_type& Elem) const {
		tree_node* Current = Root;
		while (Current) {
			if (equals_op::Equals(Current->Value, Elem)) {
				return &(Current->Value);
			}
			if (less_op::Less(Elem, Current->Value)) {
				Current = Current->LeftChild;
			} else {
				Current = Current->RightChild;
			}
		}
		return nullptr;
	}

	FORCEINLINE element_type* LowerBound(const element_type& Elem) {
		tree_node* Current = Root;
		tree_node* Result = nullptr;
		while (Current) {
			if (equals_op::Equals(Current->Value, Elem)) {
				return &(Current->Value);
			}
			if (less_op::Less(Elem, Current->Value)) {
				Current = Current->LeftChild;
			} else {
				Result = Result && !less_op::Less(Result->Value, Current->Value) ? Result : Current;
				Current = Current->RightChild;
			}
		}
		return Result ? &(Result->Value) : nullptr;
	}

	FORCEINLINE element_type* UpperBound(const element_type& Elem) {
		tree_node* Current = Root;
		tree_node* Result = nullptr;
		while (Current) {
			if (equals_op::Equals(Current->Value, Elem)) {
				return &(Current->Value);
			}
			if (less_op::Less(Current->Value, Elem)) {
				Current = Current->RightChild;
			} else {
				Result = Result && less_op::Less(Result->Value, Current->Value) ? Result : Current;
				Current = Current->LeftChild;
			}
		}
		return Result ? &(Result->Value) : nullptr;
	}

	FORCEINLINE iter begin() {
		return iter(Root);
	}

	FORCEINLINE iter rbegin() {
		return iter(Root, const_iter::iter_direction::reversed);
	}

	FORCEINLINE iter end() {
		return iter(nullptr);
	}

	FORCEINLINE iter rend() {
		return iter(nullptr);
	}

	FORCEINLINE const_iter begin() const {
		return const_iter(Root);
	}

	FORCEINLINE const_iter rbegin() const {
		return const_iter(Root, const_iter::iter_direction::reversed);
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
		Stack[0] = Root;
		for (tree_node** StackTop = Stack; StackTop >= Stack; --StackTop) {
			tree_node* Current = *StackTop;
			while (Current) {
				tree_node* NodeToDelete = Current;
				if (NodeToDelete->LeftChild) {
					if (NodeToDelete->RightChild) {
						*StackTop = NodeToDelete->RightChild;
						++StackTop;
					}
					Current = NodeToDelete->LeftChild;
				} else {
					Current = NodeToDelete->RightChild;
				}
				NodeToDelete->~tree_node();
				alloc_base::Allocator.Free(NodeToDelete);
			}
		}
		Root = nullptr;
		Size = 0;
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
