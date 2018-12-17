#pragma once
#include "undoable/Tracked.h"
#include "undoable/History.h"
#include "intrusive/List.h"

namespace undoable {

class Factory {
public:
	Factory() = default;
	~Factory();

	template<typename Type, typename... Args> Type& Create(Args&&... args);
	History& GetHistory();

private:
	History history_;
	intrusive::List<Tracked> objects_;
};

} // namespace undoable

#include "undoable/Factory-inl.h"
