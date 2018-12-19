#include <cassert>
#include <iostream>
#include "undoable/Tracked.h"


namespace undoable {


// TrackedNode

TrackedNode::TrackedNode() {
	next_ = this;
	prev_ = this;
}

TrackedNode::~TrackedNode() {
	next_->prev_ = prev_;
	prev_->next_ = next_;
}

void TrackedNode::Link(TrackedNode* u, TrackedNode* v) {
	u->next_ = v;
	v->prev_ = u;
}


// Tracked

Tracked::~Tracked() {
	assert(status_ != Status::kConstructed &&
		"Object was not created through Factory");
	assert(status_ == Status::kDestructed &&
		"Object was not destructed through Destroy()");
}

void Tracked::ApplyPropertyChange(UniquePtr<Command> command) {
	if (history_) {
		if (status_ == Status::kCreated) {
			history_->Stage(std::move(command));
		} else {
			assert(status_ != Status::kOnCreate &&
				"Cannot change property in OnCreate()");
			assert(status_ != Status::kOnDestroy &&
				"Cannot change property in OnDestroy()");
			assert(status_ != Status::kDestroyed &&
				"Cannot change property on a destroyed object");
		}
	} else {
		command->Apply(false);
	}
}

void Tracked::DestroyMembers() {
	ResetAllProperty();
}

void Tracked::Destroy() {
	if (status_ == Status::kCreated) {
		DestroyMembers();
		history_->Stage(MakeUnique<StatusChange>(this, false));
	} else {
		assert(status_ != Status::kOnCreate &&
			"Cannot destroy in OnCreate()");
		assert(status_ != Status::kOnDestroy &&
			"Cannot destroy in OnDestroy()");
		assert(status_ != Status::kDestroyed &&
			"Cannot destroy a destroyed object");
	}
}

void Tracked::Destruct(Tracked* obj) {
	// Note: destructor can freely change properties just like the ctor
	obj->history_ = nullptr;
	obj->status_ = Status::kDestructed;
	delete obj;
}

void Tracked::Init(History* history) {
	history_ = history;
	history_->Stage(MakeUnique<StatusChange>(this, true));
}


// Tracked::StatusChange

Tracked::StatusChange::StatusChange(Tracked* obj, bool create)
	: obj_(obj)
	, create_(create)
	, destructable_(false)
{}

Tracked::StatusChange::~StatusChange() {
	if (destructable_) {
		Tracked::Destruct(obj_);
	}
}

void Tracked::StatusChange::Apply(bool reverse) {
	if (create_ ^ reverse) {
		destructable_ = false;
		obj_->status_ = Status::kOnCreate;
		obj_->OnCreate();
		obj_->status_ = Status::kCreated;
	} else {
		destructable_ = true;
		obj_->status_ = Status::kOnDestroy;
		obj_->OnDestroy();
		obj_->status_ = Status::kDestroyed;
	}
}


} // namespace undoable
