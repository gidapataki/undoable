#include "TestUtils.h"
#include "undoable/Object.h"
#include "undoable/Factory.h"
#include "undoable/ValueProperty.h"
#include "undoable/RefProperty.h"


using namespace undoable;

namespace {

class Element
	: public Object
{
public:
	RefProperty<Element> next{this};
};

} // namespace


TEST(RefPropertyTest, MultipleReferences) {
	Factory f;
	auto& h = f.GetHistory();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();

	EXPECT_FALSE(e1.next);
	EXPECT_FALSE(e2.next);
	h.Commit();

	e1.next.Set(&e2);
	e2.next.Set(&e2);
	EXPECT_TRUE(e1.next);
	EXPECT_TRUE(e2.next);
	EXPECT_EQ(&e2, &*e1.next);
	EXPECT_EQ(&e2, &*e2.next);
	h.Commit();

	h.Undo();
	EXPECT_FALSE(e1.next);
	EXPECT_FALSE(e2.next);

	h.Redo();
	EXPECT_EQ(&e2, &*e1.next);
	EXPECT_EQ(&e2, &*e2.next);
}

TEST(RefPropertyTest, ResetOnDestroy) {
	Factory f;
	auto& h = f.GetHistory();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();

	e1.next.Set(&e2);
	h.Commit();
	EXPECT_EQ(&e2, &*e1.next);

	e2.Destroy();
	EXPECT_EQ((void*) nullptr, &*e1.next);
	h.Commit();

	h.Undo();
	EXPECT_EQ(&e2, &*e1.next);
}
