#pragma once
#include <list>
#include "undoable/History.h"
#include "undoable/Property.h"
#include "undoable/ListProperty.h"


namespace undoable {

class Tracked;
class TrackedNode;
template<typename T> class Object;


class TrackedNode {
public:
	TrackedNode();
	~TrackedNode();

private:
	friend class Factory;
	static void Link(TrackedNode* u, TrackedNode* v);

	TrackedNode* next_;
	TrackedNode* prev_;
};


class Tracked
	: public PropertyOwner
	, public TrackedNode
{
public:
	void Destroy();

	virtual void OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void DestroyMembers();

private:
	friend class Factory;
	template<typename T> friend class Object;

	Tracked() = default;
	~Tracked();

	enum class Status {
		kConstructed,
		kOnCreate,
		kCreated,
		kOnDestroy,
		kDestroyed,
		kDestructed,
	};

	void Init(History* history);
	static void Destruct(Tracked* obj);

	virtual void ApplyPropertyChange(UniquePtr<Command> command) override;

	class StatusChange : public Command {
	public:
		StatusChange(Tracked* obj, bool create);
		virtual ~StatusChange();
		virtual void Apply(bool reverse) override;

	private:
		Tracked* obj_;
		bool create_;
	};

	History* history_ = nullptr;
	Status status_ = Status::kConstructed;
};


template<typename T>
class Object
	: public Tracked
{
public:
	virtual void OnCreate() override {}
	virtual void OnDestroy() override {}
	virtual void OnPropertyChange(Property* property) override {}

	virtual void DestroyMembers() override {
		StaticRegistry<T, tag_Unlinker>::Get().NotifyAll(this);
		Tracked::DestroyMembers();
	}
};

} // namespace undoable
