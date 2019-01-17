#pragma once
#include "undoable/Property.h"
#include <type_traits>


namespace undoable {

class RefNode;
class Referable;
class RefPropertyBase;

template<typename Type> class RefProperty;


class RefNode {
public:
	RefNode() = default;
	~RefNode() {
		UnlinkRef();
	}

protected:
	friend class Referable;
	friend class RefPropertyBase;

	void UnlinkRef();
	static void LinkRef(RefNode* u, RefNode* v);

	RefNode* next_ref_{this};
	RefNode* prev_ref_{this};
	Referable* referable_{nullptr};
};


class Referable {
public:
	Referable() = default;

protected:
	friend class RefPropertyBase;
	void LinkBack(RefPropertyBase* ref);
	void ResetAllReferences();

private:
	RefNode head_;
};


class RefPropertyBase
	: public Property
	, public RefNode
{
public:
	RefPropertyBase(PropertyOwner* owner);
	virtual void OnReset() override;

protected:
	friend class Referable;

	void SetReferable(Referable* referable);
	void SetReferableInternal(Referable* referable);

	class Change : public Command {
	public:
		Change(RefPropertyBase* property, Referable* value)
			: property_(property)
			, value_(value)
		{}

		virtual void Apply(bool reverse) override {
			auto old_value = property_->referable_;
			property_->SetReferableInternal(value_);
			value_ = old_value;
			property_->NotifyOwner();
		}

	private:
		RefPropertyBase* property_;
		Referable* value_;
	};
};


template<typename Type>
class RefProperty
	: public RefPropertyBase
{
public:
	using RefPropertyBase::RefPropertyBase;

	void Set(Type* value);
	explicit operator bool() const;
	Type& operator*();
	Type* operator->();
	const Type& operator*() const;
	const Type* operator->() const;
};


template<typename Type>
void RefProperty<Type>::Set(Type* value) {
	static_assert(std::is_base_of<Referable, Type>::value,
		"Invalid type");
	SetReferable(value);
}

template<typename Type>
RefProperty<Type>::operator bool() const {
	return !!referable_;
}

template<typename Type>
Type& RefProperty<Type>::operator*() {
	return *static_cast<Type*>(referable_);
}

template<typename Type>
Type* RefProperty<Type>::operator->() {
	return static_cast<Type*>(referable_);
}

template<typename Type>
const Type& RefProperty<Type>::operator*() const {
	return *static_cast<const Type*>(referable_);
}

template<typename Type>
const Type* RefProperty<Type>::operator->() const {
	return static_cast<const Type*>(referable_);
}


} // namespace undoable
