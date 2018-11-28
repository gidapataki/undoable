#pragma once


namespace undoable {

template<typename T>
ValueProperty<T>::ValueProperty(PropertyOwner* owner, T value)
	: Property(owner)
	, value_(std::move(value))
{
	owner_->RegisterProperty(this);
}

template<typename T>
const T& ValueProperty<T>::Get() const {
	return value_;
}

template<typename T>
void ValueProperty<T>::Set(T value) {
	auto cmd = MakeUnique<Change>(this, std::move(value));
	owner_->ApplyPropertyChange(std::move(cmd));
}

template<typename T>
ValueProperty<T>::Change::Change(ValueProperty* property, T value)
	: property_(property)
	, value_(std::move(value))
{}

template<typename T>
void ValueProperty<T>::Change::Apply() {
	std::swap(property_->value_, value_);
	property_->owner_->OnPropertyChange();
}

} // namespace undoable
