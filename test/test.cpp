#include "undoable/History.h"
#include "undoable/Property.h"
#include "undoable/ListProperty.h"
#include "undoable/Tracked.h"
#include <iostream>


class Tick : public undoable::Command {
public:
	virtual void Apply() override {
		std::cout << (value_ ? ">" : "<") << std::endl;
		value_ = !value_;
	}

private:
	bool value_ = true;
};


class Print : public undoable::Command {
public:
	explicit Print(const std::string& msg)
		: forw_(msg)
		, back_(msg)
	{}

	explicit Print(const std::string& msg, const std::string& rev)
		: forw_(msg)
		, back_(rev)
	{}

	virtual void Apply() override {
		std::cout << forw_;
		std::swap(forw_, back_);
	}

private:
	std::string forw_;
	std::string back_;
};


void MakePrint(undoable::History& h, const std::string& msg) {
	h.Stage(std::make_unique<Print>("", "\n"));
	for (auto ch : msg) {
		std::string s;
		s += ch;
		h.Stage(std::make_unique<Print>(s));
	}
	h.Stage(std::make_unique<Print>("\n", ""));
}


class Shape : public undoable::Tracked {
public:
	Shape(undoable::History* history, const std::string& name)
		: undoable::Tracked(history)
		, x(this)
		, y(this)
		, name(this, name)
	{}

	void Dump() {
		std::cout << name.Get() << std::endl;
	}

	undoable::ValueProperty<int> x;
	undoable::ValueProperty<int> y;
	undoable::ValueProperty<std::string> name;
};


class Item
	: public undoable::Tracked
	, public undoable::ListNode<Item>
{
public:
	Item(undoable::History* history)
		: undoable::Tracked(history)
	{}

	int x = 0;
};

class Container
	: public undoable::Tracked
{
public:
	Container(undoable::History* history)
		: undoable::Tracked(history)
		, ls(this)
	{}

	void Dump() {
		std::cout << "C";
		for (auto& item : ls) {
			std::cout << " " << item.x;
		}
		std::cout << " ." << std::endl;
	}

	undoable::ListProperty<Item> ls;
};


void TestValueProperty() {
	undoable::History h;
	Shape s(&h, "initial");
	s.Dump();	// initial

	s.name.Set("changed-1");
	h.Commit();
	s.Dump();	// changed-1

	h.Undo();
	s.Dump();	// initial

	s.name.Set("changed-2");
	s.Dump();	// changed-2

	h.Unstage();
	s.Dump();	// initial

	h.Redo();
	s.Dump();	// changed-1
}


void TestListProperty() {
	undoable::History h;

	Container c(&h);
	Container c2(&h);
	Item i1(&h);
	Item i2(&h);

	c.ls.LinkFront(i1);
	c.ls.LinkBack(i2);

	i1.x = 5;
	i2.x = 7;

	c.Dump();
	h.Commit();

	h.Undo();
	c.Dump();

	h.Redo();
	c.Dump();

	c.ls.Clear();
	h.Commit();
	c.Dump();

	h.Undo();
	c.Dump();

	// c.ls.LinkFront(i2);
	// c.Dump();
	// h.Unstage();
	// c.Dump();

	// c2.ls.LinkBack(i2);
	// c.ls.Remove(c.ls.begin());
	// c.Dump();
	// c2.Dump();
}


int main() {
	TestListProperty();
	return 0;
}
