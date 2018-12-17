#pragma once
#include <list>
#include "undoable/UniquePtr.h"
#include "undoable/Command.h"


namespace undoable {

class Transaction {
public:
	Transaction() = default;
	~Transaction();
	Transaction(const Transaction&) = delete;
	Transaction(Transaction&&) = default;
	Transaction& operator=(const Transaction&) = delete;
	Transaction& operator=(Transaction&&) = default;

	bool IsEmpty() const;
	void Apply(UniquePtr<Command> command);
	void Reverse();
	void Clear();

private:
	// Note: Commands are stored in the order they were applied.
	std::list<UniquePtr<Command>> commands_;
	bool reverse_ = false;
};


class History {
public:
	~History();
	void Clear();

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
