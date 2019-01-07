#include <cassert>
#include <iostream>
#include "undoable/Object.h"


namespace undoable {


// ObjectBase

ObjectBase::ObjectBase() {
	next_object_ = this;
	prev_object_ = this;
}

ObjectBase::~ObjectBase() {
	next_object_->prev_object_ = prev_object_;
	prev_object_->next_object_ = next_object_;
}

void ObjectBase::Link(ObjectBase* u, ObjectBase* v) {
	u->next_object_ = v;
	v->prev_object_ = u;
}

bool ObjectBase::InheritanceOrderCheck() const {
	return next_object_ == this && prev_object_ == this;
}


// Object

Object::~Object() {
	assert(status_ != Status::kConstructing &&
		"Object was not created through Factory");
	assert(status_ == Status::kDestructing &&
		"Object was not destructed through Destroy()");
}

void Object::ApplyPropertyChange(UniquePtr<Command> command) {
	assert(status_ != Status::kOnCreate &&
		"Cannot change properties in OnCreate()");
	assert(status_ != Status::kOnDestroy &&
		"Cannot change properties in OnDestroy()");
	assert(status_ != Status::kDestroyed &&
		"Cannot change properties on a destroyed object");
	assert(!on_change_ &&
		"Cannot change properties via OnPropertyChange()");

	if (!on_change_) {
		if (history_ && status_ == Status::kCreated) {
			history_->Stage(std::move(command));
		} else if (!history_) {
			command->Apply(false);
		}
	}
}

void Object::DestroyMembers() {
	ResetAllProperties();
	UnlinkAllNodes();
}

void Object::Destroy() {
	assert(history_ && "History is not set");
	assert(status_ != Status::kOnCreate &&
		"Cannot destroy in OnCreate()");
	assert(status_ != Status::kOnDestroy &&
		"Cannot destroy in OnDestroy()");
	assert(status_ != Status::kDestroyed &&
		"Cannot destroy a destroyed object");

	if (history_ && status_ == Status::kCreated) {
		DestroyMembers();
		history_->Stage(MakeUnique<StatusChange>(this, false));
	}
}

void Object::Destruct(Object* obj) {
	// Note: destructor can freely change properties just like the ctor
	obj->history_ = nullptr;
	obj->status_ = Status::kDestructing;
	delete obj;
}

void Object::Init(History* history) {
	history_ = history;
	history_->Stage(MakeUnique<StatusChange>(this, true));
}

bool Object::IsConstructing() const {
	return status_ == Status::kConstructing;
}

bool Object::IsDestructing() const {
	return status_ == Status::kDestructing;
}

bool Object::IsCreated() const {
	return status_ == Status::kCreated;
}

bool Object::IsDestroyed() const {
	return status_ == Status::kDestroyed;
}


// Object::StatusChange

Object::StatusChange::StatusChange(Object* obj, bool create)
	: obj_(obj)
	, create_(create)
	, destructable_(false)
{}

Object::StatusChange::~StatusChange() {
	if (destructable_) {
		Object::Destruct(obj_);
	}
}

void Object::StatusChange::Apply(bool reverse) {
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
