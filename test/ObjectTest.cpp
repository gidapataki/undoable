#include "TestUtils.h"
#include "undoable/Object.h"
#include "undoable/Factory.h"
#include "undoable/ValueProperty.h"
#include <vector>

using namespace undoable;

namespace {

enum class EventType {
	kCreate,
	kDestroy,
	kChange,
	kDestruct,
};

struct Event {
	EventType type;
	union {
		Object* object;
		Property* property;
	};
};

Event MakeCreateEvent(Object* object) {
	Event ev;
	ev.type = EventType::kCreate;
	ev.object = object;
	return ev;
}

Event MakeDestroyEvent(Object* object) {
	Event ev;
	ev.type = EventType::kDestroy;
	ev.object = object;
	return ev;
}

Event MakeDestructEvent(Object* object) {
	Event ev;
	ev.type = EventType::kDestruct;
	ev.object = object;
	return ev;
}

Event MakeChangeEvent(Property* property) {
	Event ev;
	ev.type = EventType::kChange;
	ev.property = property;
	return ev;
}

bool operator==(const Event& lhs, const Event& rhs) {
	return lhs.type == rhs.type && lhs.object == rhs.object;
}

std::ostream& ToStream(std::ostream& stream, const Event& ev) {
	char sym = '?';
	switch (ev.type) {
		case EventType::kCreate: sym = '+'; break;
		case EventType::kDestroy: sym = '-'; break;
		case EventType::kDestruct: sym = '~'; break;
		case EventType::kChange: sym = '@'; break;
	}

	stream << sym << ev.object;
	return stream;
}

using Events = std::vector<Event>;


template<typename T>
class BeforeObject {
public:
	BeforeObject() {
		before = static_cast<T*>(this)->InheritanceOrderCheck();
	}

	bool before = false;
};

template<typename T>
class AfterObject {
public:
	AfterObject() {
		after = static_cast<T*>(this)->InheritanceOrderCheck();
	}

	bool after = false;
};

class Element
	: public BeforeObject<Element>
	, public Object
	, public AfterObject<Element>
	, public ListNode<Element, struct tag_children>
{
public:
	Element(Events& evs)
		: events(evs)
	{
		EXPECT_TRUE(IsConstructing());
		EXPECT_FALSE(change_in_ctor_);
		value.Set(3);
		EXPECT_TRUE(change_in_ctor_);
	}

	~Element() {
		EXPECT_TRUE(IsDestructing());
		EXPECT_FALSE(change_in_dtor_);
		value.Set(-3);
		EXPECT_TRUE(change_in_dtor_);
		events.push_back(MakeDestructEvent(this));
	}

	virtual void OnCreate() override {
		events.push_back(MakeCreateEvent(this));
	}

	virtual void OnDestroy() override {
		events.push_back(MakeDestroyEvent(this));
	}

	virtual void OnPropertyChange(Property* property) override {
		if (IsConstructing()) {
			change_in_ctor_ = true;
			return;
		}
		if (IsDestructing()) {
			change_in_dtor_ = true;
			return;
		}
		events.push_back(MakeChangeEvent(property));
	}

	ValueProperty<int> value {this};
	ListProperty<Element, struct tag_children> children {this};
	Events& events;
	bool change_in_ctor_ = false;
	bool change_in_dtor_ = false;
};

} // namespace

TEST(ObjectTest, InheritanceOrder) {
	Events evs;
	Factory f;
	auto& e1 = f.Create<Element>(evs);

	EXPECT_FALSE(e1.before);
	EXPECT_TRUE(e1.after);
}

TEST(ObjectTest, CreateDestroy) {
	Events evs;
	auto factory = MakeUnique<Factory>();
	auto& f = *factory;

	auto& e1 = f.Create<Element>(evs);
	EXPECT_EQ(Events{MakeCreateEvent(&e1)}, evs);

	evs.clear();
	e1.Destroy();
	EXPECT_EQ(Events{MakeDestroyEvent(&e1)}, evs);

	evs.clear();
	auto& e2 = f.Create<Element>(evs);
	EXPECT_EQ(Events{MakeCreateEvent(&e2)}, evs);

	evs.clear();
	f.GetHistory().Commit();
	EXPECT_TRUE(evs.empty());

	factory.reset();
	EXPECT_EQ((Events{
		MakeDestructEvent(&e1),
		MakeDestructEvent(&e2)
	}), evs);
}

TEST(ObjectTest, Change) {
	Events evs;
	auto factory = MakeUnique<Factory>();
	auto& f = *factory;

	auto& e1 = f.Create<Element>(evs);
	EXPECT_EQ(Events{MakeCreateEvent(&e1)}, evs);

	evs.clear();
	e1.value.Set(12);
	EXPECT_EQ(Events{MakeChangeEvent(&e1.value)}, evs);

	evs.clear();
	auto& e2 = f.Create<Element>(evs);
	e1.children.LinkBack(e2);
	EXPECT_EQ((Events{
		MakeCreateEvent(&e2),
		MakeChangeEvent(&e1.children)
	}), evs);

	evs.clear();
	f.GetHistory().Commit();
	EXPECT_TRUE(evs.empty());

	factory.reset();
	EXPECT_EQ((Events{
		MakeDestructEvent(&e1),
		MakeDestructEvent(&e2)
	}), evs);
}

TEST(ObjectTest, UndoRedo) {
	Events evs;
	auto factory = MakeUnique<Factory>();
	auto& f = *factory;
	auto& h = f.GetHistory();

	auto& e1 = f.Create<Element>(evs);
	auto& e2 = f.Create<Element>(evs);
	EXPECT_EQ(2, evs.size());

	e1.Destroy();
	e2.Destroy();

	EXPECT_EQ((Events{
		MakeCreateEvent(&e1),
		MakeCreateEvent(&e2),
		MakeDestroyEvent(&e1),
		MakeDestroyEvent(&e2),
	}), evs);

	evs.clear();

	EXPECT_TRUE(h.CanCommit());
	h.Commit();

	EXPECT_TRUE(evs.empty());
	EXPECT_TRUE(h.CanUndo());
	h.Undo();

	EXPECT_EQ((Events{
		MakeCreateEvent(&e2),
		MakeCreateEvent(&e1),
		MakeDestroyEvent(&e2),
		MakeDestroyEvent(&e1),
	}), evs);

	EXPECT_FALSE(h.CanCommit());
	EXPECT_FALSE(h.CanUndo());
	EXPECT_TRUE(h.CanRedo());

	evs.clear();
	h.Redo();

	EXPECT_EQ((Events{
		MakeCreateEvent(&e1),
		MakeCreateEvent(&e2),
		MakeDestroyEvent(&e1),
		MakeDestroyEvent(&e2),
	}), evs);

	evs.clear();

	factory.reset();
	EXPECT_EQ((Events{
		MakeDestructEvent(&e1),
		MakeDestructEvent(&e2)
	}), evs);
}

TEST(ObjectTest, DestroyMembers) {
	Events evs;
	Factory f;

	auto& e1 = f.Create<Element>(evs);
	auto& e2 = f.Create<Element>(evs);

	EXPECT_EQ(2, evs.size());

	e1.value.Set(13);
	e1.children.LinkBack(e2);
	EXPECT_EQ(4, evs.size());

	evs.clear();
	e1.Destroy();
	EXPECT_EQ((Events{
		MakeChangeEvent(&e1.children),
		MakeDestroyEvent(&e1)
	}), evs);

	f.GetHistory().Commit();
}

TEST(ObjectTest, StatusCheck) {
	Events evs;
	Factory f;

	auto& e1 = f.Create<Element>(evs);

	EXPECT_TRUE(e1.IsCreated());
	EXPECT_FALSE(e1.IsDestroyed());

	e1.Destroy();
	EXPECT_FALSE(e1.IsCreated());
	EXPECT_TRUE(e1.IsDestroyed());
}

TEST(ObjectTest, DestructInaccessible) {
	Events evs;
	Factory f;
	auto& h = f.GetHistory();
	auto& e1 = f.Create<Element>(evs);

	EXPECT_EQ((Events{MakeCreateEvent(&e1)}), evs);

	evs.clear();
	h.Commit();
	EXPECT_TRUE(evs.empty());

	auto& e2 = f.Create<Element>(evs);
	h.Unstage();
	EXPECT_EQ((Events{
		MakeCreateEvent(&e2),
		MakeDestroyEvent(&e2),
		MakeDestructEvent(&e2),
	}), evs);

	evs.clear();
	auto& e3 = f.Create<Element>(evs);
	h.Commit();
	h.Clear();
	EXPECT_EQ((Events{
		MakeCreateEvent(&e3),
	}), evs);

	evs.clear();
	e1.Destroy();
	e3.Destroy();
	h.Commit();
	EXPECT_EQ((Events{
		MakeDestroyEvent(&e1),
		MakeDestroyEvent(&e3),
	}), evs);

	evs.clear();
	h.Clear();
	EXPECT_EQ((Events{
		MakeDestructEvent(&e1),
		MakeDestructEvent(&e3),
	}), evs);
}
