#include "undoable/Factory.h"


namespace undoable {

Factory::~Factory() {
	history_.Clear();
	for (auto* p = NextTracked(); p; p = NextTracked()) {
		Tracked::Destruct(static_cast<Tracked*>(p));
	}
}

History& Factory::GetHistory() {
	return history_;
}

void Factory::LinkBack(TrackedNode* node) {
	TrackedNode::Link(head_.prev_, node);
	TrackedNode::Link(node, &head_);
}

TrackedNode* Factory::NextTracked() {
	if (head_.next_ == &head_) {
		return nullptr;
	}
	return head_.next_;
}

} // namespace undoable
