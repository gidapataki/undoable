#pragma once
#include <list>
#include "undoable/UniquePtr.h"
#include "undoable/Command.h"

namespace undoable {

class Transaction;
class History;


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

	/**
	 * Reverts pending changes and clears Undo/Redo stacks.
	 */
	void Clear();

	/**
	 * Adds a command to the pending changes.
	 */
	void Stage(UniquePtr<Command> command);

	/**
	 * Reverts pending changes.
	 */
	void Unstage();

	/**
	 * Commits pending changes and clears the Redo stack.
	 */
	void Commit();

	/**
	 * If there are no pending changes then restores the previous commit point.
	 */
	void Undo();

	/**
	 * If there are no pending changes then restores the next commit point.
	 */
	void Redo();

	/**
	 * True, if the Undo stack is not empty, and there are no pending changes.
	 */
	bool CanUndo() const;

	/**
	 * True, if the Redo stack is not empty, and there are no pending changes.
	 */
	bool CanRedo() const;

	/**
	 * True, if there are pending changes.
	 */
	bool CanCommit() const;

private:
	void ClearUndo();
	void ClearRedo();

	std::list<Transaction> undo_;
	std::list<Transaction> redo_;
	Transaction stage_;
};

} // namespace undoable
