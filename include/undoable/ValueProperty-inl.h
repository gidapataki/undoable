#pragma once


namespace undoable {

template<typename T>
ValueProperty<T>::ValueProperty(PropertyOwner* owner, T value)
	: Property(owner)
	, value_(std::move(value))
{}

template<typename T>
const T& ValueProperty<T>::Get() const {
	return value_;
}

template<typename T>
void ValueProperty<T>::Set(T value) {
	if (value != value_) {
		auto cmd = MakeUnique<Change>(this, std::move(value));
		owner_->ApplyPropertyChange(std::move(cmd));
	}
}

template<typename T>
ValueProperty<T>::Change::Change(ValueProperty* property, T value)
	: property_(property)
	, value_(std::move(value))
{}

template<typename T>
void ValueProperty<T>::Change::Apply(bool reverse) {
	std::swap(property_->value_, value_);
	property_->owner_->OnPropertyChange(property_);
}

} // namespace undoable
