#include "undoable/Registry.h"


namespace undoable {

void Registry::NotifyAll(void* userdata) {
	for (auto* p = first_; p; p = p->next_) {
		p->Notify(userdata);
	}
}

void Registry::Add(Registered* node) {
	if (!last_) {
		first_ = last_ = node;
	} else {
		last_->next_ = node;
		last_ = node;
	}
}

} // namespace undoable
