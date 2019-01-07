#include "undoable/ListProperty.h"


namespace undoable {


// ListNodeBase

ListNodeBase::ListNodeBase()
	: next_(this)
	, prev_(this)
	, parent_(nullptr)
{}

ListNodeBase::~ListNodeBase() {
	next_->prev_ = prev_;
	prev_->next_ = next_;
}

bool ListNodeBase::IsLinked() const {
	return next_ != this;
}

PropertyOwner* ListNodeBase::Owner() {
	return parent_->owner_;
}

void ListNodeBase::Link(ListNodeBase* u, ListNodeBase* v) {
	u->next_ = v;
	v->prev_ = u;
}

void ListNodeBase::Unlink() {
	if (parent_ == nullptr) {
		return;
	}

	auto cmd = MakeUnique<Relink>(this, this, nullptr);
	Owner()->ApplyPropertyChange(std::move(cmd));
}


// ListNodeBase::Relink

ListNodeBase::Relink::Relink(
		ListNodeBase* node, ListNodeBase* next, ListPropertyBase* parent)
	: node_(node)
	, next_(next)
	, parent_(parent)
{}

void ListNodeBase::Relink::Apply(bool reverse) {
	std::swap(node_->parent_, parent_);

	auto other_next = node_->next_;
	auto other_parent = node_->parent_;

	Link(node_->prev_, node_->next_);
	if (next_ != node_) {
		Link(next_->prev_, node_);
	}
	Link(node_, next_);
	next_ = other_next;

	if (parent_) {
		// The old parent is notified first
		parent_->NotifyOwner();
	}
	if (other_parent && parent_ != other_parent) {
		// The current parent is notified second,
		// so the node disappears first and then reappears.
		// If the same list is used, then we only notify once.
		other_parent->NotifyOwner();
	}
}


// ListNodeOwner

void ListNodeOwner::RegisterListNode(ListNodeBase* node) {
	if (last_node_) {
		last_node_->next_node_ = node;
		last_node_ = node;
	} else {
		first_node_ = node;
		last_node_ = node;
	}
}

void ListNodeOwner::UnlinkAllNodes() {
	for (auto* p = first_node_; p; p = p->next_node_) {
		p->Unlink();
	}
}


// ListPropertyBase

ListPropertyBase::ListPropertyBase(PropertyOwner* owner)
	: Property(owner)
{}

} // namespace undoable
