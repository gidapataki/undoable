#include "undoable/Property.h"


namespace undoable {


// Property

Property::Property(PropertyOwner* owner)
	: owner_(owner)
{
	owner->RegisterProperty(this);
}


// PropertyOwner

void PropertyOwner::RegisterProperty(Property* property) {
	if (last_property_) {
		last_property_->next_property_ = property;
		last_property_ = property;
	} else {
		first_property_ = property;
		last_property_ = property;
	}
}

void PropertyOwner::ResetAllProperties() {
	for (auto* p = first_property_; p; p = p->next_property_) {
		p->OnReset();
	}
}

} // namespace undoable
