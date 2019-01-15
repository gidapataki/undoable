#pragma once
#include "undoable/Property.h"

namespace undoable {

class Fragment
	: public PropertyOwner
	, public Property
{
public:
	Fragment(PropertyOwner* owner);
	~Fragment();

	virtual void OnReset() override;
	virtual void OnPropertyChange(Property* property) override;
	virtual void ApplyPropertyChange(UniquePtr<Command> command) override;
};

} // namespace undoable
