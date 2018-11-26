#pragma once
#include <list>
#include "undoable/History.h"
#include "undoable/Property.h"


namespace undoable {


class Tracked : public IPropertyOwner {
public:
	Tracked(History* history)
		: history_(history)
	{}

	virtual void OnPropertyChange() override {}
	virtual void ApplyPropertyChange(UniquePtr<Command> command) override {
		history_->Stage(std::move(command));
	}

protected:
	History* history_;
};

} // namespace undoable
