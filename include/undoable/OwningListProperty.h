#include "undoable/Object.h"
#include "undoable/ListProperty.h"
#include <type_traits>


namespace undoable {

template<typename Type, typename Tag>
class OwningListProperty
	: public ListProperty<Type, Tag>
{
public:
	static_assert(std::is_base_of<Object, Type>::value, "Invalid type");

	OwningListProperty(PropertyOwner* owner);
	virtual void OnReset() override;
};

} // namespace undoable

#include "undoable/OwningListProperty-inl.h"
