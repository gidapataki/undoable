#pragma once
#include "undoable/UniquePtr.h"
#include "undoable/Command.h"

namespace undoable {

class Property;
class PropertyOwner;
template<typename T> class ValueProperty;


class Property {
public:
	Property(PropertyOwner* owner);
	virtual ~Property() = default;
	virtual void Reset() {}

protected:
	friend class PropertyOwner;

	Property* next_property_ = nullptr;
	PropertyOwner* owner_ = nullptr;
};


class PropertyOwner {
public:
	virtual ~PropertyOwner() = default;
	virtual void OnPropertyChange(Property* property) = 0;
	virtual void ApplyPropertyChange(UniquePtr<Command> command) = 0;
	void RegisterProperty(Property* property);
	void ResetProperties();

protected:
	Property* first_property_ = nullptr;
	Property* last_property_ = nullptr;
};


template<typename T>
class ValueProperty
	: public Property {
public:
	ValueProperty(PropertyOwner* owner, T value=T());
	const T& Get() const;
	void Set(T value);

private:
	class Change : public Command {
	public:
		Change(ValueProperty* property, T value);
		virtual void Apply(bool reverse) override;

	private:
		ValueProperty* property_;
		T value_;
	};

	T value_;
};

} // namespace undoable

#include "undoable/Property-inl.h"
