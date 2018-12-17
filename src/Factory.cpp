#include "undoable/Factory.h"


namespace undoable {

Factory::~Factory() {
	history_.Clear();
	while (!objects_.IsEmpty()) {
		auto& obj = objects_.Front();
		Tracked::Destruct(&obj);
	}
}

History& Factory::GetHistory() {
	return history_;
}

} // namespace undoable
