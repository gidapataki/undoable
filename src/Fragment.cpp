#include "undoable/Fragment.h"

namespace undoable {

Fragment::Fragment(PropertyOwner* owner)
	: Property(owner)
{}

Fragment::~Fragment() {
}

void Fragment::OnReset() {
	ResetAllProperties();
}

void Fragment::OnPropertyChange(Property* property) {
	// Note: by default we don't propagate the change of the actual property,
	// just for the `Fragment`.
	owner_->OnPropertyChange(this);
}

void Fragment::ApplyPropertyChange(UniquePtr<Command> command) {
	owner_->ApplyPropertyChange(std::move(command));
}

} // namespace undoable
