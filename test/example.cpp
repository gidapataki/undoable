#include "undoable/History.h"
#include "undoable/ValueProperty.h"
#include "undoable/ListProperty.h"
#include "undoable/OwningListProperty.h"
#include "undoable/Object.h"
#include "undoable/Factory.h"
#include <iostream>


class Tick : public undoable::Command {
public:
	virtual void Apply(bool reverse) override {
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

	virtual void Apply(bool reverse) override {
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


class Shape : public undoable::Object {
public:
	Shape(const std::string& name)
		: x(this)
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
	: public undoable::Object
	, public undoable::ListNode<Item, struct tag_0>
	, public undoable::ListNode<Item, struct tag_1>
{
public:
	Item() = default;

	int x = 0;
};


class Item2
	: public Item
	, public undoable::ListNode<Item2, struct tag_2>
{};


class Container
	: public undoable::Object
{
public:
	Container()
		: ls(this)
	{}

	void Dump(int k) {
		std::cout << k << ") C";
		for (auto& item : ls) {
			std::cout << " " << item.x;
		}
		std::cout << " ." << std::endl;
	}

	undoable::OwningListProperty<Item, struct tag_0> ls;
};


void TestValueProperty() {
	undoable::Factory f;
	auto& h = f.GetHistory();
	auto& s = f.Create<Shape>("initial");
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
	undoable::Factory f;
	auto& h = f.GetHistory();

	auto& c = f.Create<Container>();
	auto& c2 = f.Create<Container>();
	auto& i1 = f.Create<Item>();
	auto& i2 = f.Create<Item>();

	c.ls.LinkFront(i1);
	c.ls.LinkBack(i2);

	i1.x = 5;
	i2.x = 7;

	c.Dump(__LINE__); // 5 7 .
	h.Commit();

	h.Undo();
	c.Dump(__LINE__); // .

	h.Redo();
	c.Dump(__LINE__); // 5 7 .

	c.ls.Clear();
	h.Commit();
	c.Dump(__LINE__); // .

	h.Undo();
	c.Dump(__LINE__); // 5 7 .

	c.ls.LinkFront(i2);
	c.Dump(__LINE__); // 7 5 .
	h.Unstage();
	c.Dump(__LINE__); // 5 7 .

	c2.ls.LinkBack(i2);
	c.ls.Remove(c.ls.begin());
	c.Dump(__LINE__); // .
	c2.Dump(__LINE__); // 7 .

	h.Commit();
	c.ls.LinkBack(i1);
	c.Dump(__LINE__); // 5 .

	i1.Destroy();

	h.Commit();
	c.Dump(__LINE__); // .

	f.Create<Item2>();
}


int main2() {
	TestListProperty();
	return 0;
}
