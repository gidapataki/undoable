#include "TestUtils.h"
#include "undoable/Object.h"
#include "undoable/Factory.h"
#include "undoable/OwningListProperty.h"
#include <vector>

using namespace undoable;

namespace {

class Element
	: public Object
	, public ListNode<Element, struct tag_owner>
	, public ListNode<Element, struct tag1>
{
public:
	using OwnerNodeNode = ListNode<Element, struct tag_owner>;
	using AuxNode = ListNode<Element, struct tag1>;

	OwningListProperty<Element, struct tag_owner> children {this};
	ListProperty<Element, struct tag1> aux {this};
};

} // namespace


TEST(OwningListPropertyTest, Lifecycle) {
	Factory f;
	auto& h = f.GetHistory();

	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();

	e1.children.LinkBack(e2);
	e2.children.LinkBack(e3);
	e2.aux.LinkBack(e1);

	h.Commit();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_TRUE(e3.IsCreated());
	EXPECT_TRUE(e1.AuxNode::IsLinked());
	EXPECT_EQ(1, e2.aux.Size());

	e2.Destroy();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsDestroyed());
	EXPECT_TRUE(e3.IsDestroyed());
	EXPECT_FALSE(e1.AuxNode::IsLinked());

	h.Unstage();
	e3.Destroy();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_TRUE(e3.IsDestroyed());
	EXPECT_TRUE(e1.AuxNode::IsLinked());

	e1.Destroy();
	h.Commit();
	EXPECT_TRUE(e1.IsDestroyed());
	EXPECT_TRUE(e2.IsDestroyed());
	EXPECT_TRUE(e3.IsDestroyed());
}

TEST(OwningListPropertyTest, CircularReference) {
	Factory f;
	auto& h = f.GetHistory();

	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();

	e1.children.LinkBack(e2);
	e2.children.LinkBack(e3);
	e3.children.LinkBack(e1);
	h.Commit();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_TRUE(e3.IsCreated());

	e2.Destroy();
	EXPECT_TRUE(e1.IsDestroyed());
	EXPECT_TRUE(e2.IsDestroyed());
	EXPECT_TRUE(e3.IsDestroyed());

	h.Unstage();
	EXPECT_TRUE(e1.IsCreated());
	EXPECT_TRUE(e2.IsCreated());
	EXPECT_TRUE(e3.IsCreated());

	e3.Destroy();
	EXPECT_TRUE(e1.IsDestroyed());
	EXPECT_TRUE(e2.IsDestroyed());
	EXPECT_TRUE(e3.IsDestroyed());
}
