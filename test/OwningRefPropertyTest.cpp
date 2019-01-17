#include "TestUtils.h"
#include "undoable/Object.h"
#include "undoable/Factory.h"
#include "undoable/ValueProperty.h"
#include "undoable/RefProperty.h"
#include "undoable/OwningRefProperty.h"
#include "undoable/OwningListProperty.h"

using namespace undoable;

namespace {

class Element
	: public Object
	, public ListNode<Element, struct tag_children>
{
public:
	OwningRefProperty<Element> child{this};
	OwningRefProperty<Element> other{this};
	OwningListProperty<Element, struct tag_children> children{this};
};

} // namespace

TEST(OwningRefPropertyTest, Propagation) {
	Factory f;
	auto& h = f.GetHistory();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();

	e1.child.Set(&e2);
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_EQ(&e2, &*e1.child);
	h.Commit();

	e2.Destroy();
	EXPECT_EQ((void*) nullptr, &*e1.child);

	h.Unstage();
	EXPECT_TRUE(e2.IsCreated());

	e1.Destroy();
	h.Commit();
	EXPECT_FALSE(e1.IsCreated());
	EXPECT_FALSE(e2.IsCreated());

	h.Undo();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_EQ(&e2, &*e1.child);
}


TEST(OwningRefPropertyTest, MultipleOwnership) {
	Factory f;
	auto& h = f.GetHistory();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();
	auto& e4 = f.Create<Element>();

	e1.child.Set(&e3);
	e2.child.Set(&e3);
	e3.other.Set(&e4);
	h.Commit();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_TRUE(e3.IsCreated());
	EXPECT_TRUE(e4.IsCreated());

	e1.Destroy();
	h.Commit();
	EXPECT_FALSE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_FALSE(e3.IsCreated());
	EXPECT_FALSE(e4.IsCreated());

	h.Undo();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_TRUE(e3.IsCreated());
	EXPECT_TRUE(e4.IsCreated());
	EXPECT_EQ(&e4, &*e3.other);

	e2.Destroy();
	h.Commit();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_FALSE(e2.IsCreated());
	EXPECT_FALSE(e3.IsCreated());
	EXPECT_FALSE(e4.IsCreated());
}


TEST(OwningRefPropertyTest, CircularOwnership) {
	Factory f;
	auto& h = f.GetHistory();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();
	auto& e4 = f.Create<Element>();

	e1.child.Set(&e2);
	e2.children.LinkBack(e3);
	e2.children.LinkBack(e4);
	e3.child.Set(&e1);
	h.Commit();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_TRUE(e3.IsCreated());
	EXPECT_TRUE(e4.IsCreated());

	e1.Destroy();
	EXPECT_FALSE(e1.IsCreated());
	EXPECT_FALSE(e2.IsCreated());
	EXPECT_FALSE(e3.IsCreated());
	EXPECT_FALSE(e4.IsCreated());

	h.Unstage();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_TRUE(e3.IsCreated());
	EXPECT_TRUE(e4.IsCreated());

	e2.Destroy();
	EXPECT_FALSE(e1.IsCreated());
	EXPECT_FALSE(e2.IsCreated());
	EXPECT_FALSE(e3.IsCreated());
	EXPECT_FALSE(e4.IsCreated());

	h.Unstage();
	e3.Destroy();
	EXPECT_FALSE(e1.IsCreated());
	EXPECT_FALSE(e2.IsCreated());
	EXPECT_FALSE(e3.IsCreated());
	EXPECT_FALSE(e4.IsCreated());
}
