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

} // namespace undoable
