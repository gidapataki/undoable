#include "undoable/RefProperty.h"


namespace undoable {


// RefNode

void RefNode::LinkRef(RefNode* u, RefNode* v) {
	u->next_ref_ = v;
	v->prev_ref_ = u;
}

void RefNode::UnlinkRef() {
	LinkRef(prev_ref_, next_ref_);
	LinkRef(this, this);
	referable_ = nullptr;
}


// Referable

void Referable::LinkBack(RefPropertyBase* node) {
	RefNode::LinkRef(node->prev_ref_, node->next_ref_);
	RefNode::LinkRef(head_.prev_ref_, node);
	RefNode::LinkRef(node, &head_);
	node->referable_ = this;
}

void Referable::ResetAllReferences() {
	for (auto* p = head_.next_ref_; p != &head_; p = head_.next_ref_) {
		// Note: calling OnReset() could lead to a destroy loop
		static_cast<RefPropertyBase*>(p)->SetReferable(nullptr);
	}
}

// RefPropertyBase

RefPropertyBase::RefPropertyBase(PropertyOwner* owner)
	: Property(owner)
{}

void RefPropertyBase::SetReferable(Referable* referable) {
	if (referable != referable_) {
		auto cmd = MakeUnique<Change>(this, referable);
		owner_->ApplyPropertyChange(std::move(cmd));
	}
}

void RefPropertyBase::SetReferableInternal(Referable* referable) {
	if (referable) {
		referable->LinkBack(this);
	} else {
		UnlinkRef();
	}
}

void RefPropertyBase::OnReset() {
	SetReferable(nullptr);
}

} // namespace undoable
