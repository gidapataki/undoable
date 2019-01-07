#pragma once
#include <vector>

namespace undoable {

template<typename Type, typename Tag>
OwningListProperty<Type, Tag>::OwningListProperty(PropertyOwner* owner)
	: ListProperty<Type, Tag>(owner)
{}

template<typename Type, typename Tag>
void OwningListProperty<Type, Tag>::OnReset() {
	std::vector<Type*> vec;
	vec.reserve(ListProperty<Type, Tag>::Size());
	for (auto& obj : *this) {
		vec.push_back(&obj);
	}
	ListProperty<Type, Tag>::Clear();
	for (auto p : vec) {
		p->Destroy();
	}
}

} // namespace undoable
