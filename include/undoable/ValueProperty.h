#pragma once
#include "undoable/Property.h"
#include "undoable/Command.h"


namespace undoable {

template<typename T>
class ValueProperty
	: public Property {
public:
	ValueProperty(PropertyOwner* owner, T value=T());
	virtual void OnReset() override {}

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

#include "undoable/ValueProperty-inl.h"
