#pragma once
#include <type_traits>

namespace undoable {

template<typename Type, typename... Args>
Type& Factory::Create(Args&&... args) {
	static_assert(
		std::is_base_of<Object<Type>, Type>::value,
		"Missing base class");

	Type* obj = new Type(args...);
	objects_.LinkBack(*obj);
	obj->Init(&history_);
	return *obj;
}

} // namespace undoable
