#pragma once
#include <list>
#include "undoable/History.h"
#include "undoable/Property.h"
#include "undoable/ListProperty.h"


namespace undoable {

class ObjectBase {
public:
	ObjectBase();
	~ObjectBase();

	bool InheritanceOrderCheck() const;

protected:
	friend class Factory;
	static void Link(ObjectBase* u, ObjectBase* v);

	ObjectBase* next_object_;
	ObjectBase* prev_object_;
};


class Object
	: public ObjectBase
	, public PropertyOwner
	, public ListNodeOwner
{
public:
	Object() = default;
	~Object();

	void Destroy();

	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void OnPropertyChange(Property* property) override {}

private:
	friend class Factory;

	enum class Status {
		kConstructed,
		kOnCreate,
		kCreated,
		kOnDestroy,
		kDestroyed,
		kDestructed,
	};

	class StatusChange : public Command {
	public:
		StatusChange(Object* obj, bool create);
		virtual ~StatusChange();
		virtual void Apply(bool reverse) override;

	private:
		Object* obj_;
		bool create_;
		bool destructable_;
	};

	void Init(History* history);
	void DestroyMembers();
	static void Destruct(Object* obj);
	virtual void ApplyPropertyChange(UniquePtr<Command> command) override;

	History* history_ = nullptr;
	Status status_ = Status::kConstructed;
};

} // namespace undoable
