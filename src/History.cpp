#include "undoable/History.h"

namespace undoable {


// Transaction

Transaction::~Transaction() {
	Clear();
}

void Transaction::Clear() {
	// Note: destruction order is important
	for (auto& cmd : commands_) {
		cmd = nullptr;
	}
	commands_.clear();
}

bool Transaction::IsEmpty() const {
	return commands_.empty();
}

void Transaction::Apply(UniquePtr<Command> command) {
	command->Apply();
	commands_.push_back(std::move(command));
}

void Transaction::ReverseApply() {
	commands_.reverse();
	for (auto& cmd : commands_) {
		cmd->Apply();
	}
}


// History

History::~History() {
	Clear();
}

void History::Stage(UniquePtr<Command> command) {
	stage_.Apply(std::move(command));
}

void History::Unstage() {
	stage_.ReverseApply();
	stage_.Clear();
}

void History::Commit() {
	if (stage_.IsEmpty()) {
		// Empty commits are not allowed
		return;
	}

	undo_.emplace_back(std::move(stage_));
	stage_ = {};

	ClearRedo();
}

void History::Undo() {
	if (undo_.empty()) {
		return;
	}

	undo_.back().ReverseApply();

	auto it = undo_.end();
	--it;
	redo_.splice(redo_.begin(), undo_, it);
}

void History::Redo() {
	if (redo_.empty()) {
		return;
	}

	redo_.front().ReverseApply();
	undo_.splice(undo_.end(), redo_, redo_.begin());
}

void History::ClearRedo() {
	while (!redo_.empty()) {
		redo_.pop_back();
	}
}

void History::ClearUndo() {
	while (!undo_.empty()) {
		undo_.pop_front();
	}
}

void History::Clear() {
	Unstage();
	ClearUndo();
	ClearRedo();
}

} // namespace undoable
