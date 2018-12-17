#pragma once

namespace undoable {

class Command {
public:
	virtual ~Command() = default;
	virtual void Apply(bool reverse) = 0;
};

} // namespace undoable
