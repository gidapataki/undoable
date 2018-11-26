#pragma once
#include "undoable/UniquePtr.h"


namespace undoable {


class IPropertyOwner {
public:
	/*
	 * Abstract class for classes with properties.
	 * Note that recording a change in History is the responsibility
	 * of the implementing class. These classes has to have virtual functions
	 * anyway, so omitting this abstraction layer would not help.
	 */
	virtual void OnPropertyChange() = 0;
	virtual void ApplyPropertyChange(UniquePtr<Command> command) = 0;
};


template<typename T>
class ValueProperty {
public:
	class Change : public Command {
	public:
		Change(ValueProperty* property, T value)
			: property_(property)
			, value_(std::move(value))
		{}

		virtual void Apply() override {
			std::swap(property_->value_, value_);
			property_->owner_->OnPropertyChange();
		}

	private:
		ValueProperty* property_;
		T value_;
	};

	ValueProperty(IPropertyOwner* owner, T value=T())
		: owner_(owner)
		, value_(std::move(value))
	{}

	const T& Get() const {
		return value_;
	}

	void Set(T value) {
		auto cmd = MakeUnique<Change>(this, std::move(value));
		owner_->ApplyPropertyChange(std::move(cmd));
	}

private:
	IPropertyOwner* owner_;
	T value_;
};


} // namespace undoable
