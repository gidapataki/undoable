#include "undoable/RefProperty.h"
#include "undoable/Object.h"
#include <type_traits>

namespace undoable {

template<typename Type>
class OwningRefProperty
	: public RefProperty<Type>
{
public:
	using RefProperty<Type>::RefProperty;

	virtual void OnReset() override;
};


template<typename Type>
void OwningRefProperty<Type>::OnReset() {
	static_assert(std::is_base_of<Object, Type>::value, "Invalid type");
	auto referable = RefPropertyBase::referable_;
	if (referable) {
		RefPropertyBase::SetReferable(nullptr);
		static_cast<Object*>(referable)->Destroy();
	}
}

} // namespace undoable
