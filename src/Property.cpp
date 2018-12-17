#include "undoable/Property.h"


namespace undoable {


Property::Property(PropertyOwner* owner)
	: owner_(owner)
{}

void PropertyOwner::RegisterProperty(Property* property) {
	if (!last_property_) {
		first_property_ = property;
		last_property_ = property;
	} else {
		last_property_->next_property_ = property;
		last_property_ = property;
	}
}

void PropertyOwner::ResetProperties() {
	for (auto* p = first_property_; p; p = p->next_property_) {
		p->Reset();
	}
}

} // namespace undoable
