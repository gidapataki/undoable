#include "undoable/Tracked.h"
#include "undoable/ListProperty.h"
#include <type_traits>

#include <vector>


namespace undoable {

template<typename Type, typename Tag>
class OwningListProperty
	: public ListProperty<Type, Tag>
{
public:
	static_assert(std::is_base_of<Object<Type>, Type>::value, "Invalid type");

	OwningListProperty(PropertyOwner* owner);
	virtual void OnReset() override;
};


template<typename Type, typename Tag>
OwningListProperty<Type, Tag>::OwningListProperty(PropertyOwner* owner)
	: ListProperty<Type, Tag>(owner)
{}


template<typename Type, typename Tag>
void OwningListProperty<Type, Tag>::OnReset() {
	std::vector<Type*> vec;
	for (auto& obj : *this) {
		vec.push_back(&obj);
	}
	ListProperty<Type, Tag>::Clear();
	for (auto p : vec) {
		p->Destroy();
	}
}

} // namespace undoable