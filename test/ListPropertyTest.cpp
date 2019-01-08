#include "TestUtils.h"
#include "undoable/Object.h"
#include "undoable/Factory.h"
#include "undoable/ListProperty.h"

using namespace undoable;

namespace {

class Element
	: public Object
	, public ListNode<Element, struct tag0>
	, public ListNode<Element, struct tag1>
{
public:
	int value = 0;
};

using XList = ListProperty<Element, struct tag0>;
using YList = ListProperty<Element, struct tag1>;

class Container
	: public Object
{
public:
	Container()
		: ls0(this)
		, ls1(this)
	{}

	XList ls0;
	YList ls1;
};

template<typename Tag>
std::vector<Element*> ToVector(ListProperty<Element, Tag>& list) {
	std::vector<Element*> vec;
	for (auto& e : list) {
		vec.push_back(&e);
	}
	return vec;
}

} // namespace

template<>
void ToStream(std::ostream& stream, const XList::iterator& it) {
	stream << &*it;
}

template<>
void ToStream(std::ostream& stream, const XList::const_iterator& it) {
	stream << &*it;
}

TEST(ListPropertyTest, Sizes) {
	Factory f;
	auto& c = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();
	auto& list = c.ls0;


	EXPECT_TRUE(list.IsEmpty());
	EXPECT_EQ(0, list.Size());

	list.LinkBack(e1);
	EXPECT_FALSE(list.IsEmpty());
	EXPECT_EQ(1, list.Size());

	list.LinkBack(e2);
	EXPECT_FALSE(list.IsEmpty());
	EXPECT_EQ(2, list.Size());

	list.LinkBack(e3);
	EXPECT_FALSE(list.IsEmpty());
	EXPECT_EQ(3, list.Size());

	list.UnlinkBack();
	EXPECT_FALSE(list.IsEmpty());
	EXPECT_EQ(2, list.Size());

	list.Clear();
	EXPECT_TRUE(list.IsEmpty());
	EXPECT_EQ(0, list.Size());
}

TEST(ListPropertyTest, Link) {
	Factory f;
	auto& c = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();
	auto& list = c.ls0;

	list.LinkBack(e1);
	list.LinkBack(e2);
	list.LinkFront(e3);

	EXPECT_EQ(&e3, &list.Front());
	EXPECT_EQ(&e2, &list.Back());
}

TEST(ListPropertyTest, LinkUnlink) {
	Factory f;
	auto& c = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();
	auto& list = c.ls0;

	list.LinkBack(e1);
	list.LinkBack(e2);
	list.LinkBack(e3);
	EXPECT_EQ(&e1, &list.Front());
	EXPECT_EQ(&e3, &list.Back());

	list.UnlinkBack();
	EXPECT_EQ(&e1, &list.Front());
	EXPECT_EQ(&e2, &list.Back());

	list.UnlinkFront();
	EXPECT_EQ(&e2, &list.Front());
	EXPECT_EQ(&e2, &list.Back());
}

TEST(ListPropertyTest, Remove) {
	Factory f;
	auto& c = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();
	auto& e4 = f.Create<Element>();
	auto& list = c.ls0;

	list.LinkBack(e1);
	list.LinkBack(e2);
	list.LinkBack(e3);

	list.Remove(list.Find(e2));
	EXPECT_EQ(&e1, &list.Front());
	EXPECT_EQ(&e3, &list.Back());

	list.Remove(list.Find(e1));
	EXPECT_EQ(&e3, &list.Front());
	EXPECT_EQ(&e3, &list.Back());

	list.Remove(list.Find(e3));
	EXPECT_TRUE(list.IsEmpty());

	list.Remove(list.Find(e4));
	EXPECT_TRUE(list.IsEmpty());
}

TEST(ListPropertyTest, Count) {
	Factory f;
	auto& c1 = f.Create<Container>();
	auto& c2 = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();
	auto& e4 = f.Create<Element>();
	auto& list1 = c1.ls0;
	auto& list2 = c2.ls0;

	list1.LinkBack(e1);
	list1.LinkBack(e2);
	list2.LinkBack(e3);

	EXPECT_EQ(1, list1.Count(e1));
	EXPECT_EQ(1, list1.Count(e2));
	EXPECT_EQ(0, list1.Count(e3));

	EXPECT_EQ(0, list2.Count(e1));
	EXPECT_EQ(0, list2.Count(e2));
	EXPECT_EQ(1, list2.Count(e3));
}

TEST(ListPropertyTest, Iterators) {
	Factory f;
	auto& c = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& list = c.ls0;

	XList::iterator li;
	XList::iterator lj;


	list.LinkBack(e1);
	list.LinkBack(e2);

	lj = li = list.begin();
	EXPECT_EQ(&*li, &e1);
	EXPECT_EQ(&*lj, &e1);
	EXPECT_EQ(li, lj);

	lj = ++li;
	EXPECT_EQ(&*li, &e2);
	EXPECT_EQ(&*lj, &e2);

	lj = li++;
	EXPECT_EQ(li, list.end());
	EXPECT_EQ(&*lj, &e2);

	lj = li--;
	EXPECT_EQ(&*li, &e2);
	EXPECT_EQ(lj, list.end());

	lj = --li;
	EXPECT_EQ(&*li, &e1);
	EXPECT_EQ(&*lj, &e1);
}

TEST(ListPropertyTest, ConstIterators) {
	Factory f;
	auto& c = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& list = c.ls0;

	XList::const_iterator li;
	XList::const_iterator lj;

	list.LinkBack(e1);
	list.LinkBack(e2);

	lj = li = list.cbegin();
	EXPECT_EQ(&*li, &e1);
	EXPECT_EQ(&*lj, &e1);
	EXPECT_EQ(li, lj);

	lj = ++li;
	EXPECT_EQ(&*li, &e2);
	EXPECT_EQ(&*lj, &e2);

	lj = li++;
	EXPECT_EQ(li, list.cend());
	EXPECT_EQ(&*lj, &e2);

	lj = li--;
	EXPECT_EQ(&*li, &e2);
	EXPECT_EQ(lj, list.cend());

	lj = --li;
	EXPECT_EQ(&*li, &e1);
	EXPECT_EQ(&*lj, &e1);
}

TEST(ListPropertyTest, Containment) {
	Factory f;
	auto& c1 = f.Create<Container>();
	auto& c2 = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& list1 = c1.ls0;
	auto& list2 = c2.ls0;

	list1.LinkBack(e1);
	list1.LinkBack(e2);
	EXPECT_EQ(&e1, &list1.Front());
	EXPECT_EQ(&e2, &list1.Back());

	list2.LinkBack(e1);
	EXPECT_EQ(&e2, &list1.Front());
	EXPECT_EQ(&e2, &list1.Back());
	EXPECT_EQ(&e1, &list2.Front());
	EXPECT_EQ(&e1, &list2.Back());

	list2.LinkBack(e2);
	EXPECT_TRUE(list1.IsEmpty());
	EXPECT_TRUE(list1.IsEmpty());
	EXPECT_EQ(&e1, &list2.Front());
	EXPECT_EQ(&e2, &list2.Back());
}

TEST(ListPropertyTest, MultipleContainment) {
	Factory f;
	auto& c = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& list1 = c.ls0;
	auto& list2 = c.ls1;

	list1.LinkBack(e1);
	list1.LinkBack(e2);
	EXPECT_EQ(&e1, &list1.Front());
	EXPECT_EQ(&e2, &list1.Back());

	list2.LinkBack(e1);
	EXPECT_EQ(&e1, &list1.Front());
	EXPECT_EQ(&e2, &list1.Back());
	EXPECT_EQ(&e1, &list2.Front());
	EXPECT_EQ(&e1, &list2.Back());

	list2.LinkBack(e2);
	EXPECT_EQ(&e1, &list1.Front());
	EXPECT_EQ(&e2, &list1.Back());
	EXPECT_EQ(&e1, &list2.Front());
	EXPECT_EQ(&e2, &list2.Back());
}

TEST(ListPropertyTest, RangeIterator) {
	using Vector = std::vector<Element*>;
	using ConstVector = std::vector<const Element*>;

	Factory f;
	auto& c = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();
	auto& e4 = f.Create<Element>();
	auto& list = c.ls0;

	list.LinkBack(e1);
	list.LinkFront(e2);
	list.LinkBack(e3);
	list.LinkFront(e4);

	Vector vec0;
	ConstVector vec1;

	for (auto& e : list) {
		vec0.push_back(&e);
		vec1.push_back(&e);
	}

	EXPECT_EQ(Vector({&e4, &e2, &e1, &e3}), vec0);
	EXPECT_EQ(ConstVector({&e4, &e2, &e1, &e3}), vec1);
}

TEST(ListPropertyTest, Insertion) {
	using Vector = std::vector<Element*>;

	Factory f;
	auto& c = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();
	auto& e4 = f.Create<Element>();
	auto& list = c.ls0;

	XList::iterator it, it2;

	list.LinkBack(e1);
	list.LinkBack(e2);
	list.LinkBack(e3);
	EXPECT_EQ(Vector({ &e1, &e2, &e3 }), ToVector(list));

	it = list.begin();
	it2 = list.LinkAt(it, e4);
	EXPECT_EQ(Vector({ &e4, &e1, &e2, &e3 }), ToVector(list));
	EXPECT_EQ(&e4, &*it2);

	it = list.begin();
	EXPECT_EQ(&e4, &*it);

	it2 = list.LinkAt(it, e4);
	EXPECT_EQ(Vector({ &e4, &e1, &e2, &e3 }), ToVector(list));
	EXPECT_EQ(&e4, &*it2);

	++it;
	EXPECT_EQ(&e1, &*it);
	EXPECT_EQ(&e4, &*it2);

	it2 = list.LinkAt(it, e4);
	EXPECT_EQ(Vector({ &e4, &e1, &e2, &e3 }), ToVector(list));
	EXPECT_EQ(&e4, &*it2);

	++it;
	EXPECT_EQ(&e2, &*it);

	it2 = list.LinkAt(it, e4);
	EXPECT_EQ(Vector({ &e1, &e4, &e2, &e3 }), ToVector(list));
	EXPECT_EQ(&e4, &*it2);

	++it;
	EXPECT_EQ(&e3, &*it);

	list.LinkAt(it, e4);
	EXPECT_EQ(Vector({ &e1, &e2, &e4, &e3 }), ToVector(list));

	++it;
	EXPECT_EQ(list.end(), it);

	it2 = list.LinkAt(it, e2);
	EXPECT_EQ(Vector({ &e1, &e4, &e3, &e2 }), ToVector(list));
	EXPECT_EQ(&e2, &*it2);
}

TEST(ListPropertyTest, Find) {
	Factory f;
	auto& c1 = f.Create<Container>();
	auto& c2 = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& e3 = f.Create<Element>();
	auto& e4 = f.Create<Element>();
	auto& list = c1.ls0;
	auto& list2 = c2.ls0;

	const XList& const_list = list;
	XList::iterator it;
	XList::const_iterator const_it;

	list.LinkBack(e1);
	list.LinkBack(e2);
	list.LinkBack(e3);
	list2.LinkBack(e4);

	it = list.begin();
	++it;

	const_it = const_list.begin();
	++const_it;

	EXPECT_EQ(it, XList::iterator(&e2));
	EXPECT_EQ(const_it, XList::const_iterator(&e2));
	EXPECT_EQ(const_list.cbegin(), const_list.begin());
	EXPECT_EQ(const_list.cend(), const_list.end());

	EXPECT_EQ(list.end(), list.Find(e4));
	EXPECT_EQ(const_list.end(), const_list.Find(e4));
	EXPECT_EQ(list2.begin(), list2.Find(e4));

	list.Remove(list.Find(e3));
	EXPECT_EQ(list.end(), list.Find(e3));
}

TEST(ListPropertyTest, Destroy) {
	using Vector = std::vector<Element*>;

	Factory f;
	auto& c = f.Create<Container>();
	auto& e1 = f.Create<Element>();
	auto& e2 = f.Create<Element>();
	auto& list = c.ls0;

	list.LinkBack(e1);
	list.LinkBack(e2);
	EXPECT_EQ(Vector({&e1, &e2}), ToVector(list));

	e2.Destroy();
	EXPECT_EQ(Vector({&e1}), ToVector(list));
}


TEST(ListPropertyTest, UndoRedo) {
	// fixme -
}