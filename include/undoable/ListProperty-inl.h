#pragma once
#include <cassert>


namespace undoable {


// ListNode

template<typename Type, typename Tag>
ListNode<Type, Tag>::ListNode()
	: next_(this)
	, prev_(this)
	, parent_(nullptr)
{
	auto* registry = &StaticRegistry<Type, tag_Unlinker>::Get();
	static Unlinker unlinker(registry);
}

template<typename Type, typename Tag>
ListNode<Type, Tag>::~ListNode() {
	// fixme implement
}

template<typename Type, typename Tag>
void ListNode<Type, Tag>::Unlink() {
	if (parent_ == nullptr) {
		return;
	}

	auto cmd = MakeUnique<Relink>(this, this, nullptr);
	parent_->owner_->ApplyPropertyChange(std::move(cmd));
}

template<typename Type, typename Tag>
bool ListNode<Type, Tag>::IsLinked() const {
	return next_ != this;
}

template<typename Type, typename Tag>
Type* ListNode<Type, Tag>::Object() {
	return static_cast<Type*>(this);
}

template<typename Type, typename Tag>
const Type* ListNode<Type, Tag>::Object() const {
	return static_cast<const Type*>(this);
}

template<typename Type, typename Tag>
void ListNode<Type, Tag>::Link(ListNode* u, ListNode* v) {
	u->next_ = v;
	v->prev_ = u;
}

template<typename Type, typename Tag>
ListNode<Type, Tag>& ListNode<Type, Tag>::Next(ListNode& node) {
	return *node.next_;
}

template<typename Type, typename Tag>
ListNode<Type, Tag>& ListNode<Type, Tag>::Prev(ListNode& node) {
	return *node.prev_;
}

template<typename Type, typename Tag>
ListNode<Type, Tag>* ListNode<Type, Tag>::Next(ListNode* node) {
	return node->next_;
}

template<typename Type, typename Tag>
ListNode<Type, Tag>* ListNode<Type, Tag>::Prev(ListNode* node) {
	return node->prev_;
}


// ListNode::Relink

template<typename Type, typename Tag>
ListNode<Type, Tag>::Relink::Relink(
		ListNode* node, ListNode* next, ListProperty* parent)
	: node_(node)
	, next_(next)
	, parent_(parent)
{}

template<typename Type, typename Tag>
void ListNode<Type, Tag>::Relink::Apply(bool reverse) {
	std::swap(node_->parent_, parent_);

	auto other_next = Next(node_);
	auto other_parent = node_->parent_;

	Link(Prev(node_), Next(node_));
	if (next_ != node_) {
		Link(Prev(next_), node_);
	}
	Link(node_, next_);
	next_ = other_next;

	if (parent_) {
		// The old parent is notified first
		parent_->owner_->OnPropertyChange(parent_);
	}
	if (other_parent && parent_ != other_parent) {
		// The current parent is notified second,
		// so the node disappears first and then reappears.
		// If the same list is used, then we only notify once.
		other_parent->owner_->OnPropertyChange(other_parent);
	}
}


// ListNode::Unlinker

template<typename Type, typename Tag>
ListNode<Type, Tag>::Unlinker::Unlinker(Registry* registry) {
	registry->Add(this);
}

template<typename Type, typename Tag>
void ListNode<Type, Tag>::Unlinker::Notify(void* userdata) {
	auto* obj = reinterpret_cast<Type*>(userdata);
	static_cast<ListNode*>(obj)->Unlink();
}


// ListIterator

template<typename Type, typename Tag>
ListIterator<Type, Tag>::ListIterator(ListNode* node)
	: node_(node)
{}

template<typename Type, typename Tag>
ListIterator<Type, Tag>& ListIterator<Type, Tag>::operator++() {
	node_ = ListNode::Next(node_);
	return *this;
}

template<typename Type, typename Tag>
ListIterator<Type, Tag>& ListIterator<Type, Tag>::operator--() {
	node_ = node_->prev_;
	return *this;
}

template<typename Type, typename Tag>
ListIterator<Type, Tag> ListIterator<Type, Tag>::operator++(int) {
	ListIterator it = *this;
	node_ = node_->next_;
	return it;
}

template<typename Type, typename Tag>
ListIterator<Type, Tag> ListIterator<Type, Tag>::operator--(int) {
	ListIterator it = *this;
	node_ = node_->prev_;
	return it;
}

template<typename Type, typename Tag>
bool ListIterator<Type, Tag>::operator==(const ListIterator& other) const {
	return other.node_ == node_;
}

template<typename Type, typename Tag>
bool ListIterator<Type, Tag>::operator!=(const ListIterator& other) const {
	return other.node_ != node_;
}

template<typename Type, typename Tag>
Type* ListIterator<Type, Tag>::operator->() const {
	return node_->Object();
}

template<typename Type, typename Tag>
Type& ListIterator<Type, Tag>::operator*() const {
	return *node_->Object();
}


// ListProperty

template<typename Type, typename Tag>
ListProperty<Type, Tag>::ListProperty(PropertyOwner* owner)
	: Property(owner)
{}

template<typename Type, typename Tag>
ListProperty<Type, Tag>::~ListProperty()
{}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::OnReset() {
	Clear();
}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::UnlinkFront() {
	Next(head_).Unlink();
}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::UnlinkBack() {
	Prev(head_).Unlink();
}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::LinkAt(iterator pos, ListNode& u) {
	assert((pos.node_ == &head_ || pos.node_->parent == this) &&
		"Invalid iterator");
	auto cmd = MakeUnique<typename ListNode::Relink>(&u, pos.node_, this);
	owner_->ApplyPropertyChange(std::move(cmd));
}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::LinkFront(ListNode& u) {
	auto cmd = MakeUnique<typename ListNode::Relink>(
		&u, &ListNode::Next(head_), this);
	owner_->ApplyPropertyChange(std::move(cmd));
}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::LinkBack(ListNode& u) {
	auto cmd = MakeUnique<typename ListNode::Relink>(&u, &head_, this);
	owner_->ApplyPropertyChange(std::move(cmd));
}

template<typename Type, typename Tag>
bool ListProperty<Type, Tag>::IsEmpty() const {
	return !head_.IsLinked();
}

template<typename Type, typename Tag>
Type& ListProperty<Type, Tag>::Front() {
	return *Next(head_).Object();
}

template<typename Type, typename Tag>
Type& ListProperty<Type, Tag>::Back() {
	return *head_.prev_->Object();
}

template<typename Type, typename Tag>
const Type& ListProperty<Type, Tag>::Front() const {
	return *Next(head_)->Object();
}

template<typename Type, typename Tag>
const Type& ListProperty<Type, Tag>::Back() const {
	return *head_.prev_->Object();
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::iterator ListProperty<Type, Tag>::begin() {
	return iterator(&ListNode::Next(head_));
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::iterator ListProperty<Type, Tag>::end() {
	return iterator(&head_);
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::const_iterator ListProperty<Type, Tag>::begin() const {
	return const_iterator(&Next(head_));
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::const_iterator ListProperty<Type, Tag>::end() const {
	return const_iterator(&head_);
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::const_iterator ListProperty<Type, Tag>::cbegin() const {
	return const_iterator(&Next(head_));
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::const_iterator ListProperty<Type, Tag>::cend() const {
	return const_iterator(&head_);
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::iterator ListProperty<Type, Tag>::Remove(iterator it) {
	if (it.node_ != &head_) {
		assert(it.node_->parent_ == this && "Node is in a different list");
		auto next = it;
		++next;
		it.node_->Unlink();
		return next;
	}
	return it;
}

template<typename Type, typename Tag>
std::size_t ListProperty<Type, Tag>::Size() const {
	std::size_t size = 0;
	for (auto it = begin(), it_end = end(); it != it_end; ++it) {
		++size;
	}
	return size;
}

template<typename Type, typename Tag>
std::size_t ListProperty<Type, Tag>::Count(const ListNode& u) const {
	auto it = Find(u);
	return it == end() ? 0 : 1;
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::iterator ListProperty<Type, Tag>::Find(const ListNode& u) {
	for (auto it = begin(), it_end = end(); it != it_end; ++it) {
		if (&*it == &u) {
			return it;
		}
	}
	return end();
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::const_iterator ListProperty<Type, Tag>::Find(const ListNode& u) const {
	for (auto it = begin(), it_end = end(); it != it_end; ++it) {
		if (&*it == &u) {
			return it;
		}
	}
	return end();
}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::Clear() {
	auto cmd = MakeUnique<ReplaceAll>(this);
	owner_->ApplyPropertyChange(std::move(cmd));
}

template<typename Type, typename Tag>
ListProperty<Type, Tag>::ReplaceAll::ReplaceAll(ListProperty* list)
	: list_(list)
{
	for (auto& it : *list_) {
		items_.push_back(&it);
	}
}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::ReplaceAll::Apply(bool reverse) {
	auto* head = &list_->head_;

	if (reverse) {
		for (auto* it : items_) {
			ListNode::Link(ListNode::Prev(head), it);
			ListNode::Link(it, head);
			it->parent_ = list_;
		}
	} else {
		ListNode::Link(head, head);
		for (auto* it : items_) {
			ListNode::Link(it, it);
			it->parent_ = nullptr;
		}
	}

	list_->owner_->OnPropertyChange(list_);
}

} // namespace
