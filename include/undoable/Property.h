#pragma once
#include "undoable/UniquePtr.h"
#include "undoable/Command.h"

namespace undoable {

class Property;
class PropertyOwner;

class Property {
public:
	Property(PropertyOwner* owner);
	virtual ~Property() = default;
	virtual void OnReset() = 0;

protected:
	PropertyOwner* owner_ = nullptr;

private:
	friend class PropertyOwner;
	Property* next_property_ = nullptr;
};


class PropertyOwner {
public:
	virtual ~PropertyOwner() = default;
	virtual void OnPropertyChange(Property* property) = 0;
	virtual void ApplyPropertyChange(UniquePtr<Command> command) = 0;

	/**
	 * Calls OnReset() on all properties.
	 */
	void ResetAllProperties();

protected:
	friend class Property;
	void RegisterProperty(Property* property);

	Property* first_property_ = nullptr;
	Property* last_property_ = nullptr;
};

} // namespace undoable
