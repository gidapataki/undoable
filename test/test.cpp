#include "undoable/History.h"
#include "undoable/Property.h"
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


int main() {
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

	return 0;
}
