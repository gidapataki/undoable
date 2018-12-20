#include "undoable/Factory.h"


namespace undoable {

Factory::~Factory() {
	history_.Clear();
	for (auto* p = NextObject(); p; p = NextObject()) {
		Object::Destruct(p);
	}
}

History& Factory::GetHistory() {
	return history_;
}

void Factory::LinkBack(ObjectBase* node) {
	ObjectBase::Link(head_.prev_object_, node);
	ObjectBase::Link(node, &head_);
}

Object* Factory::NextObject() {
	if (head_.next_object_ == &head_) {
		return nullptr;
	}
	return static_cast<Object*>(head_.next_object_);
}

} // namespace undoable
