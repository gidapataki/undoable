#pragma once
#include "undoable/Object.h"
#include "undoable/History.h"

namespace undoable {

class Factory {
public:
	Factory() = default;
	~Factory();

	template<typename Type, typename... Args> Type& Create(Args&&... args);
	History& GetHistory();

private:
	void LinkBack(ObjectBase* node);
	Object* NextObject();

	History history_;
	ObjectBase head_;
};

} // namespace undoable

#include "undoable/Factory-inl.h"
