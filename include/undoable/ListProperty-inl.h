#pragma once
#include <cassert>


namespace undoable {

// ListNode

template<typename Type, typename Tag>
ListNode<Type, Tag>::ListNode() {
	assert(static_cast<Type*>(this)->InheritanceOrderCheck() &&
		"Invalid inheritance order");

	static_cast<ListNodeOwner*>(
		static_cast<Type*>(this))->RegisterListNode(this);
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
ListNode<Type, Tag>& ListNode<Type, Tag>::Next(ListNode& node) {
	return *static_cast<ListNode*>(node.next_);
}

template<typename Type, typename Tag>
ListNode<Type, Tag>& ListNode<Type, Tag>::Prev(ListNode& node) {
	return *static_cast<ListNode*>(node.prev_);
}

template<typename Type, typename Tag>
ListNode<Type, Tag>* ListNode<Type, Tag>::Next(ListNode* node) {
	return static_cast<ListNode*>(node->next_);
}

template<typename Type, typename Tag>
ListNode<Type, Tag>* ListNode<Type, Tag>::Prev(ListNode* node) {
	return static_cast<ListNode*>(node->prev_);
}

template<typename Type, typename Tag>
const ListNode<Type, Tag>& ListNode<Type, Tag>::Next(const ListNode& node) {
	return *static_cast<const ListNode*>(node.next_);
}

template<typename Type, typename Tag>
const ListNode<Type, Tag>& ListNode<Type, Tag>::Prev(const ListNode& node) {
	return *static_cast<const ListNode*>(node.prev_);
}

template<typename Type, typename Tag>
const ListNode<Type, Tag>* ListNode<Type, Tag>::Next(const ListNode* node) {
	return static_cast<const ListNode*>(node->next_);
}

template<typename Type, typename Tag>
const ListNode<Type, Tag>* ListNode<Type, Tag>::Prev(const ListNode* node) {
	return static_cast<const ListNode*>(node->prev_);
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
	node_ = ListNode::Prev(node_);
	return *this;
}

template<typename Type, typename Tag>
ListIterator<Type, Tag> ListIterator<Type, Tag>::operator++(int) {
	ListIterator it = *this;
	node_ = ListNode::Next(node_);
	return it;
}

template<typename Type, typename Tag>
ListIterator<Type, Tag> ListIterator<Type, Tag>::operator--(int) {
	ListIterator it = *this;
	node_ = ListNode::Prev(node_);
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
	: ListPropertyBase(owner)
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
	ListNode::Next(Head()).Unlink();
}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::UnlinkBack() {
	ListNode::Prev(Head()).Unlink();
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::iterator ListProperty<Type, Tag>::LinkAt(
	iterator pos, ListNode& u)
{
	assert((pos.node_ == &Head() || pos.node_->parent_ == this) &&
		"Invalid iterator");
	if (pos.node_ != &u) {
		auto cmd = MakeUnique<typename ListNode::Relink>(&u, pos.node_, this);
		owner_->ApplyPropertyChange(std::move(cmd));
		pos.node_ = &u;
	}
	return pos;
}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::LinkFront(ListNode& u) {
	auto cmd = MakeUnique<typename ListNode::Relink>(
		&u, &ListNode::Next(Head()), this);
	owner_->ApplyPropertyChange(std::move(cmd));
}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::LinkBack(ListNode& u) {
	auto cmd = MakeUnique<typename ListNode::Relink>(&u, &Head(), this);
	owner_->ApplyPropertyChange(std::move(cmd));
}

template<typename Type, typename Tag>
bool ListProperty<Type, Tag>::IsEmpty() const {
	return !Head().IsLinked();
}

template<typename Type, typename Tag>
Type& ListProperty<Type, Tag>::Front() {
	return *ListNode::Next(Head()).Object();
}

template<typename Type, typename Tag>
Type& ListProperty<Type, Tag>::Back() {
	return *ListNode::Prev(Head()).Object();
}

template<typename Type, typename Tag>
const Type& ListProperty<Type, Tag>::Front() const {
	return *ListNode::Next(Head()).Object();
}

template<typename Type, typename Tag>
const Type& ListProperty<Type, Tag>::Back() const {
	return *ListNode::Prev(Head()).Object();
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::iterator ListProperty<Type, Tag>::begin() {
	return iterator(&ListNode::Next(Head()));
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::iterator ListProperty<Type, Tag>::end() {
	return iterator(&Head());
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::const_iterator ListProperty<Type, Tag>::begin() const {
	return const_iterator(&ListNode::Next(Head()));
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::const_iterator ListProperty<Type, Tag>::end() const {
	return const_iterator(&Head());
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::const_iterator ListProperty<Type, Tag>::cbegin() const {
	return const_iterator(&ListNode::Next(Head()));
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::const_iterator ListProperty<Type, Tag>::cend() const {
	return const_iterator(&Head());
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::iterator ListProperty<Type, Tag>::Remove(iterator it) {
	if (it.node_ != &Head()) {
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
typename ListProperty<Type, Tag>::iterator ListProperty<Type, Tag>::Find(ListNode& u) {
	if (u.parent_ == this) {
		return iterator(&u);
	}

	return end();
}

template<typename Type, typename Tag>
typename ListProperty<Type, Tag>::const_iterator ListProperty<Type, Tag>::Find(const ListNode& u) const {
	if (u.parent_ == this) {
		return const_iterator(&u);
	}

	return end();
}

template<typename Type, typename Tag>
void ListProperty<Type, Tag>::Clear() {
	if (!IsEmpty()) {
		auto cmd = MakeUnique<ReplaceAll>(this);
		owner_->ApplyPropertyChange(std::move(cmd));
	}
}

template<typename Type, typename Tag>
ListNode<Type, Tag>& ListProperty<Type, Tag>::Head() {
	return static_cast<ListNode&>(head_);
}

template<typename Type, typename Tag>
const ListNode<Type, Tag>& ListProperty<Type, Tag>::Head() const {
	return static_cast<const ListNode&>(head_);
}


// ListProperty<Type, Tag>::ReplaceAll

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
	auto* head = &list_->Head();

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

	list_->NotifyOwner();
}

} // namespace
