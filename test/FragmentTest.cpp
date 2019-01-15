#include "TestUtils.h"
#include "undoable/Object.h"
#include "undoable/Factory.h"
#include "undoable/Fragment.h"
#include "undoable/ValueProperty.h"
#include "undoable/OwningListProperty.h"
#include <iostream>

using namespace undoable;

namespace {

class Element;

class XFragment
	: public Fragment
{
public:
	using Fragment::Fragment;
	ValueProperty<int> value{this};
};

class YFragment
	: public Fragment
{
public:
	using Fragment::Fragment;
	int aux = 5;
	ValueProperty<int> value{this};
	OwningListProperty<Element, struct tag_children> children{this};
};

class Element
	: public Object
	, public ListNode<Element, struct tag_children>
{
public:
	Element(int value) {
		a.value.Set(value);
		b.value.Set(value * 2);
		c.value.Set(value * 3);
	}

	void OnPropertyChange(Property* property) {
		last_changed = property;
	}

	XFragment a{this};
	XFragment b{this};
	YFragment c{this};

	Property* last_changed = nullptr;
};

} // namespace


TEST(FragmentTest, Propagation) {
	Factory f;
	auto& h = f.GetHistory();

	auto& e1 = f.Create<Element>(1);
	auto& e2 = f.Create<Element>(2);

	EXPECT_EQ(1, e1.a.value.Get());
	EXPECT_EQ(2, e1.b.value.Get());
	EXPECT_EQ(3, e1.c.value.Get());
	EXPECT_EQ((void*) nullptr, e1.last_changed);

	e1.c.children.LinkBack(e2);
	h.Commit();

	EXPECT_EQ(&e1.c, e1.last_changed);
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());

	e1.a.value.Set(5);
	EXPECT_EQ(&e1.a, e1.last_changed);

	e1.Destroy();
	h.Commit();
	EXPECT_TRUE(e1.IsDestroyed());
	EXPECT_TRUE(e2.IsDestroyed());

	h.Undo();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_EQ(1, e2.c.children.Size());
}
