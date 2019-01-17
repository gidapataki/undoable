#pragma once
#include <list>
#include "undoable/History.h"
#include "undoable/Property.h"
#include "undoable/ListProperty.h"
#include "undoable/RefProperty.h"


namespace undoable {

class ObjectBase;
class Object;


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
	, public Referable
{
public:
	Object() = default;
	~Object();

	bool IsConstructing() const;
	bool IsDestructing() const;
	bool IsCreated() const;
	bool IsDestroyed() const;
	void Destroy();

	/**
	 * Override these functions to handle Create/Destroy/PropertyChange.
	 */
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void OnPropertyChange(Property* property) override {}

private:
	friend class Factory;

	enum class Status {
		kConstructing,
		kOnCreate,
		kCreated,
		kOnDestroy,
		kDestroyed,
		kDestructing,
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
	Status status_ = Status::kConstructing;
};

} // namespace undoable
