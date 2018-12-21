#include "TestUtils.h"
#include "undoable/History.h"
#include <vector>
#include <iostream>


using namespace undoable;

enum Change {
	kChange,
	kRevert,
	kDeleted,
};

using Event = std::pair<int, Change>;
using Events = std::vector<Event>;

template<>
void ToStream(std::ostream& stream, const Event& ev) {
	char sym = '?';
	switch (ev.second) {
		case kChange: sym = '>'; break;
		case kRevert: sym = '<'; break;
		case kDeleted: sym = '~'; break;
		default: break;
	}
	stream << ev.first << sym;
}


class Tick : public Command {
public:
	Tick(int id, Events& ev) : id_(id), events_(ev) {}
	~Tick() {
		events_.emplace_back(id_, kDeleted);
	}

	virtual void Apply(bool reverse) override {
		events_.emplace_back(id_, reverse ? kRevert : kChange);
	}

private:
	int id_ = 0;
	Events& events_;
};


TEST(HistoryTest, StageUnstage) {
	Events ev;
	History h;

	h.Stage(MakeUnique<Tick>(1, ev));
	EXPECT_EQ(MakeVector<Event>({{1, kChange}}), ev);
	EXPECT_TRUE(h.CanCommit());

	h.Unstage();
	EXPECT_EQ(MakeVector<Event>({{1, kChange}, {1, kRevert}, {1, kDeleted}}), ev);
	EXPECT_FALSE(h.CanCommit());
	EXPECT_FALSE(h.CanUndo());
	EXPECT_FALSE(h.CanRedo());

	ev.clear();
	h.Stage(MakeUnique<Tick>(2, ev));
	EXPECT_EQ(MakeVector<Event>({{2, kChange}}), ev);
}


TEST(HistoryTest, UndoRedo) {
	Events ev;
	History h;

	h.Stage(MakeUnique<Tick>(1, ev));
	h.Stage(MakeUnique<Tick>(2, ev));
	h.Commit();
	EXPECT_EQ(MakeVector<Event>({{1, kChange}, {2, kChange}}), ev);
	EXPECT_FALSE(h.CanCommit());
	EXPECT_TRUE(h.CanUndo());
	EXPECT_FALSE(h.CanRedo());

	h.Unstage();
	EXPECT_EQ(MakeVector<Event>({{1, kChange}, {2, kChange}}), ev);

	ev.clear();
	h.Undo();
	EXPECT_EQ(MakeVector<Event>({{2, kRevert}, {1, kRevert}}), ev);
	EXPECT_FALSE(h.CanCommit());
	EXPECT_FALSE(h.CanUndo());
	EXPECT_TRUE(h.CanRedo());

	h.Undo();
	EXPECT_EQ(MakeVector<Event>({{2, kRevert}, {1, kRevert}}), ev);

	ev.clear();
	h.Redo();
	EXPECT_EQ(MakeVector<Event>({{1, kChange}, {2, kChange}}), ev);
	EXPECT_FALSE(h.CanCommit());
	EXPECT_TRUE(h.CanUndo());
	EXPECT_FALSE(h.CanRedo());
}


TEST(HistoryTest, UndoRedoMore) {
	Events ev;
	History h;

	h.Stage(MakeUnique<Tick>(1, ev));
	h.Commit();
	h.Stage(MakeUnique<Tick>(2, ev));
	h.Commit();
	EXPECT_EQ(MakeVector<Event>({{1, kChange}, {2, kChange}}), ev);
	EXPECT_TRUE(h.CanUndo());
	EXPECT_FALSE(h.CanRedo());

	ev.clear();
	h.Undo();
	EXPECT_EQ(MakeVector<Event>({{2, kRevert}}), ev);
	EXPECT_TRUE(h.CanUndo());
	EXPECT_TRUE(h.CanRedo());

	ev.clear();
	h.Undo();
	EXPECT_EQ(MakeVector<Event>({{1, kRevert}}), ev);
	EXPECT_FALSE(h.CanUndo());
	EXPECT_TRUE(h.CanRedo());

	ev.clear();
	h.Redo();
	EXPECT_TRUE(h.CanRedo());
	h.Redo();
	EXPECT_EQ(MakeVector<Event>({{1, kChange}, {2, kChange}}), ev);

	EXPECT_TRUE(h.CanUndo());
	EXPECT_FALSE(h.CanRedo());
}

TEST(HistoryTest, CommitClearsRedo) {
	Events ev;
	History h;

	h.Stage(MakeUnique<Tick>(1, ev));
	h.Commit();
	h.Undo();
	EXPECT_EQ(MakeVector<Event>({{1, kChange}, {1, kRevert}}), ev);
	EXPECT_TRUE(h.CanRedo());
	EXPECT_FALSE(h.CanCommit());

	ev.clear();
	h.Stage(MakeUnique<Tick>(2, ev));
	EXPECT_FALSE(h.CanUndo());
	EXPECT_FALSE(h.CanRedo());
	EXPECT_TRUE(h.CanCommit());
	EXPECT_EQ(MakeVector<Event>({{2, kChange}}), ev);

	h.Unstage();
	EXPECT_EQ(MakeVector<Event>({{2, kChange}, {2, kRevert}, {2, kDeleted}}), ev);
	EXPECT_FALSE(h.CanUndo());
	EXPECT_TRUE(h.CanRedo());
	EXPECT_FALSE(h.CanCommit());

	ev.clear();
	h.Stage(MakeUnique<Tick>(3, ev));
	h.Commit();
	EXPECT_TRUE(h.CanUndo());
	EXPECT_FALSE(h.CanRedo());
	EXPECT_EQ(MakeVector<Event>({{3, kChange}, {1, kDeleted}}), ev);

	h.Undo();
	EXPECT_FALSE(h.CanUndo());
	EXPECT_TRUE(h.CanRedo());
	EXPECT_EQ(MakeVector<Event>({{3, kChange}, {1, kDeleted}, {3, kRevert}}), ev);
}

TEST(HistoryTest, Clear) {
	Events ev;
	History h;

	h.Stage(MakeUnique<Tick>(1, ev));
	EXPECT_EQ(MakeVector<Event>({{1, kChange}}), ev);
	EXPECT_TRUE(h.CanCommit());

	h.Clear();
	EXPECT_EQ(MakeVector<Event>({{1, kChange}, {1, kRevert}, {1, kDeleted}}), ev);
	EXPECT_FALSE(h.CanCommit());

	ev.clear();
	h.Stage(MakeUnique<Tick>(2, ev));
	h.Commit();
	EXPECT_EQ(MakeVector<Event>({{2, kChange}}), ev);
	EXPECT_TRUE(h.CanUndo());

	h.Clear();
	EXPECT_EQ(MakeVector<Event>({{2, kChange}, {2, kDeleted}}), ev);
	EXPECT_FALSE(h.CanUndo());

	ev.clear();
	h.Stage(MakeUnique<Tick>(3, ev));
	h.Commit();
	h.Undo();
	EXPECT_EQ(MakeVector<Event>({{3, kChange}, {3, kRevert}}), ev);
	EXPECT_TRUE(h.CanRedo());

	h.Clear();
	EXPECT_EQ(MakeVector<Event>({{3, kChange}, {3, kRevert}, {3, kDeleted}}), ev);
	EXPECT_FALSE(h.CanRedo());
}
