#pragma once
#include <list>
#include "undoable/UniquePtr.h"


namespace undoable {

class Command {
public:
	virtual ~Command() = default;

	// Apply() should toggle between two states.
	virtual void Apply() = 0;
};


class Transaction {
public:
	Transaction() = default;
	~Transaction();
	Transaction(const Transaction&) = delete;
	Transaction(Transaction&&) = default;
	Transaction& operator=(const Transaction&) = delete;
	Transaction& operator=(Transaction&&) = default;

	bool IsEmpty() const;
	void ReverseApply();
	void Clear();
	void Apply(UniquePtr<Command> command);

private:
	// Commands are stored in the order they were applied.
	std::list<UniquePtr<Command>> commands_;
};


class History {
public:
	~History();

	void Stage(UniquePtr<Command> command);
	void Unstage();
	void Commit();
	void Undo();
	void Redo();

private:
	void ClearUndo();
	void ClearRedo();

	std::list<Transaction> undo_;
	std::list<Transaction> redo_;
	Transaction stage_;
};


} // namespace undoable
