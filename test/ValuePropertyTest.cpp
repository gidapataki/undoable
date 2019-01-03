#include "TestUtils.h"
#include "undoable/ValueProperty.h"

using namespace undoable;

namespace {

class Store
	: public PropertyOwner
{
public:
	Store()
		: prop_int1(this)
		, prop_int2(this, 3)
		, prop_str(this)
		, prop_vec(this, {5, 7})
	{}

	virtual void ApplyPropertyChange(UniquePtr<Command> cmd) override {
		++apply_count;
		if (!readonly) {
			cmd->Apply(false);
		}
	}

	virtual void OnPropertyChange(Property* property) override {
		++handler_count;
		last_handler = property;
	}


	int apply_count = 0;
	int handler_count = 0;
	bool readonly = false;

	Property* last_handler = nullptr;

	ValueProperty<int> prop_int1;
	ValueProperty<int> prop_int2;
	ValueProperty<std::string> prop_str;
	ValueProperty<std::vector<int>> prop_vec;
};

} // namespace

TEST(ValuePropertyTest, Init) {
	Store s;

	EXPECT_EQ(0, s.prop_int1.Get());
	EXPECT_EQ(3, s.prop_int2.Get());
	EXPECT_EQ("", s.prop_str.Get());
	EXPECT_EQ(MakeVector<int>({5, 7}), s.prop_vec.Get());
}

TEST(ValuePropertyTest, Changes) {
	Store s;

	EXPECT_EQ(0, s.apply_count);
	EXPECT_EQ(0, s.handler_count);
	EXPECT_EQ((void*)nullptr, s.last_handler);

	s.prop_int1.Set(11);

	EXPECT_EQ(11, s.prop_int1.Get());
	EXPECT_EQ(1, s.apply_count);
	EXPECT_EQ(1, s.handler_count);
	EXPECT_EQ(&s.prop_int1, s.last_handler);

	s.prop_vec.Set({});
	s.prop_str.Set("hello");

	EXPECT_EQ(3, s.apply_count);
	EXPECT_EQ(3, s.handler_count);
	EXPECT_EQ(&s.prop_str, s.last_handler);

	s.readonly = true;
	s.prop_int2.Set(36);
	EXPECT_EQ(4, s.apply_count);
	EXPECT_EQ(3, s.handler_count);
	EXPECT_EQ(&s.prop_str, s.last_handler);
}

TEST(ValuePropertyTest, Equality) {
	Store s;

	EXPECT_EQ(0, s.apply_count);

	s.prop_int1.Set(12);
	EXPECT_EQ(1, s.apply_count);

	s.prop_int1.Set(12);
	EXPECT_EQ(1, s.apply_count);
}
