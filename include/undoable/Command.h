#pragma once

namespace undoable {

class Command {
public:
	virtual ~Command() = default;

	// Apply() should toggle between two states.
	virtual void Apply() = 0;
};

} // namespace undoable
