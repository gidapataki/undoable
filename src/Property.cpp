#include "undoable/Property.h"


namespace undoable {

Property::Property(PropertyOwner* owner)
	: owner_(owner)
{
	if (!owner->last_property_) {
		owner->first_property_ = this;
		owner->last_property_ = this;
	} else {
		owner->last_property_->next_property_ = this;
		owner->last_property_ = this;
	}
}

void PropertyOwner::ResetAllProperty() {
	for (auto* p = first_property_; p; p = p->next_property_) {
		p->OnReset();
	}
}

} // namespace undoable
