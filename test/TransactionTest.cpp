#include "TestUtils.h"
#include "undoable/History.h"


using namespace undoable;

int& NextDtor() {
	static int next = 0;
	++next;
	return next;
}

int& NextApply() {
	static int next = 0;
	++next;
	return next;
}

void ResetCounters() {
	NextDtor() = 0;
	NextApply() = 0;
}


class Tick : public Command {
public:
	Tick(int& dtor_order)
		: dtor_order(dtor_order)
	{}

	~Tick() {
		dtor_order = NextDtor();
	}

	virtual void Apply(bool _reverse) {
		++count;
		order = NextApply();
		reverse = _reverse;
	}

	int& dtor_order;
	int order = 0;
	int count = 0;
	bool reverse = false;
};


struct TickInfo {
	Tick* tick = nullptr;
	int dtor = 0;

	void AddTo(Transaction& t) {
		auto cmd = MakeUnique<Tick>(dtor);
		tick = cmd.get();
		t.Apply(std::move(cmd));
	}
};


TEST(TransactionTest, Init) {
	Transaction t;

	EXPECT_TRUE(t.IsEmpty());

	t.Clear();
	EXPECT_TRUE(t.IsEmpty());
}

TEST(TransactionTest, Apply) {
	ResetCounters();

	Transaction t;
	TickInfo i1;
	TickInfo i2;

	EXPECT_TRUE(t.IsEmpty());

	i1.AddTo(t);
	i2.AddTo(t);

	EXPECT_FALSE(t.IsEmpty());

	EXPECT_EQ(1, i1.tick->count);
	EXPECT_EQ(1, i2.tick->count);

	EXPECT_EQ(1, i1.tick->order);
	EXPECT_EQ(2, i2.tick->order);

	EXPECT_FALSE(i1.tick->reverse);
	EXPECT_FALSE(i2.tick->reverse);

	EXPECT_EQ(0, i1.dtor);
	EXPECT_EQ(0, i2.dtor);

	t.Clear();
	EXPECT_EQ(1, i1.dtor);
	EXPECT_EQ(2, i2.dtor);
}

TEST(TransactionTest, Reverse) {
	ResetCounters();

	Transaction t;
	TickInfo i1;
	TickInfo i2;
	TickInfo i3;

	EXPECT_TRUE(t.IsEmpty());

	i1.AddTo(t);
	i2.AddTo(t);
	// i1, i2

	EXPECT_EQ(1, i1.tick->count);
	EXPECT_EQ(1, i2.tick->count);
	EXPECT_EQ(1, i1.tick->order);
	EXPECT_EQ(2, i2.tick->order);
	EXPECT_FALSE(i1.tick->reverse);
	EXPECT_FALSE(i2.tick->reverse);

	t.Reverse();
	// i2, i1

	EXPECT_EQ(2, i1.tick->count);
	EXPECT_EQ(2, i2.tick->count);
	EXPECT_EQ(4, i1.tick->order);
	EXPECT_EQ(3, i2.tick->order);
	EXPECT_TRUE(i1.tick->reverse);
	EXPECT_TRUE(i2.tick->reverse);

	i3.AddTo(t);
	// i2, i1, i3

	EXPECT_EQ(1, i3.tick->count);
	EXPECT_EQ(5, i3.tick->order);
	EXPECT_TRUE(i3.tick->reverse);

	t.Reverse();
	// i3, i1, i2

	EXPECT_EQ(3, i1.tick->count);
	EXPECT_EQ(3, i2.tick->count);
	EXPECT_EQ(2, i3.tick->count);

	EXPECT_EQ(7, i1.tick->order);
	EXPECT_EQ(8, i2.tick->order);
	EXPECT_EQ(6, i3.tick->order);

	EXPECT_FALSE(i1.tick->reverse);
	EXPECT_FALSE(i2.tick->reverse);
	EXPECT_FALSE(i3.tick->reverse);

	t.Clear();
	EXPECT_EQ(1, i3.dtor);
	EXPECT_EQ(2, i1.dtor);
	EXPECT_EQ(3, i2.dtor);
}

TEST(TransactionTest, Destruct) {
	ResetCounters();

	TickInfo i1;
	TickInfo i2;

	{
		Transaction t;
		i1.AddTo(t);
		i2.AddTo(t);
	}

	EXPECT_EQ(1, i1.dtor);
	EXPECT_EQ(2, i2.dtor);
}
