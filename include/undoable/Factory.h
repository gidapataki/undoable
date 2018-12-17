#pragma once
#include "undoable/Tracked.h"
#include "undoable/History.h"

namespace undoable {

class Factory {
public:
	Factory() = default;
	~Factory();

	template<typename Type, typename... Args> Type& Create(Args&&... args);
	History& GetHistory();

private:
	void LinkBack(TrackedNode* node);
	TrackedNode* NextTracked();

	History history_;
	TrackedNode head_;
};

} // namespace undoable

#include "undoable/Factory-inl.h"
