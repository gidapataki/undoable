#pragma once
#include <iterator>
#include <type_traits>
#include <vector>
#include "undoable/Property.h"
#include "undoable/Command.h"
#include "undoable/Registry.h"


namespace undoable {

struct tag_Unlinker; // fixme remmve

class ListNodeBase;
class ListPropertyBase;

template<typename Type, typename Tag> class ListNode;
template<typename Type, typename Tag> class ListProperty;
template<typename Type, typename Tag> class ListIterator;


class ListNodeBase {
public:
	ListNodeBase();
	~ListNodeBase();
	ListNodeBase(const ListNodeBase&) = delete;
	ListNodeBase(ListNodeBase&&) = delete;
	ListNodeBase& operator=(const ListNodeBase&) = delete;
	ListNodeBase& operator=(ListNodeBase&&) = delete;

	bool IsLinked() const;
	void Unlink();

protected:
	class Relink
		: public Command {
	public:
		Relink(ListNodeBase* node, ListNodeBase* next, ListPropertyBase* parent);
		virtual void Apply(bool reverse) override;

	private:
		ListNodeBase* node_;
		ListNodeBase* next_;
		ListPropertyBase* parent_;
	};

	PropertyOwner* Owner();
	static void Link(ListNodeBase* u, ListNodeBase* v);

	ListNodeBase* next_;
	ListNodeBase* prev_;
	ListPropertyBase* parent_;

private:
	ListNodeBase* next_node_ = nullptr;
};


class ListNodeOwner {
public:

private:
	ListNodeBase* first_node_ = nullptr;
	ListNodeBase* last_node_ = nullptr;
};


template<typename Type, typename Tag>
class ListNode
	: public ListNodeBase
{
public:
	ListNode();

	Type* Object();
	const Type* Object() const;

private:
	friend class ListProperty<Type, Tag>;
	friend class ListIterator<Type, Tag>;
	friend class ListIterator<const Type, Tag>;

	class Unlinker : public Registered {
	public:
		Unlinker(Registry* registry);
		virtual void Notify(void* userdata) override;
	};

	static ListNode& Next(ListNode& node);
	static ListNode& Prev(ListNode& node);
	static ListNode* Next(ListNode* node);
	static ListNode* Prev(ListNode* node);
};


template<typename Type, typename Tag>
class ListIterator
	: public std::iterator<std::bidirectional_iterator_tag, Type>
{
public:
	using NonConstType = typename std::remove_const<Type>::type;
	friend class ListProperty<NonConstType, Tag>;
	using ListNode = typename std::conditional<
		std::is_const<Type>::value,
		const ListNode<NonConstType, Tag>,
		ListNode<Type, Tag>>::type;

	ListIterator() = default;
	explicit ListIterator(ListNode* node);
	ListIterator& operator++();
	ListIterator& operator--();

	ListIterator operator++(int);
	ListIterator operator--(int);

	bool operator==(const ListIterator& other) const;
	bool operator!=(const ListIterator& other) const;

	Type* operator->() const;
	Type& operator*() const;

private:
	ListNode* node_ = nullptr;
};


class ListPropertyBase
	: public Property
{
public:
	ListPropertyBase(PropertyOwner* owner);

protected:
	friend class ListNodeBase;

	ListNodeBase head_;
};

template<typename Type, typename Tag>
class ListProperty
	: public ListPropertyBase
{
public:
	friend class ListNode<Type, Tag>;

	using ListNode = ListNode<Type, Tag>;
	using iterator = ListIterator<Type, Tag>;
	using const_iterator = ListIterator<const Type, Tag>;

	ListProperty(PropertyOwner* owner);
	~ListProperty();
	virtual void OnReset() override;

	// O(1)
	void UnlinkFront();
	void UnlinkBack();
	void LinkAt(iterator pos, ListNode& u);
	void LinkFront(ListNode& u);
	void LinkBack(ListNode& u);
	bool IsEmpty() const;

	Type& Front();
	Type& Back();
	const Type& Front() const;
	const Type& Back() const;

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;

	iterator Remove(iterator it);

	// O(n)
	void Clear();
	std::size_t Count(const ListNode& u) const;
	std::size_t Size() const;
	iterator Find(const ListNode& u);
	const_iterator Find(const ListNode& u) const;

protected:
	class ReplaceAll : public Command {
	public:
		ReplaceAll(ListProperty* list);
		virtual void Apply(bool reverse) override;

	private:
		std::vector<ListNode*> items_;
		ListProperty* list_;
	};

	ListProperty(ListProperty&&) = delete;
	ListProperty(const ListProperty&) = delete;
	ListProperty& operator=(const ListProperty&) = delete;
	ListProperty& operator=(ListProperty&&) = delete;

	ListNode& Head();
};

} // namespace

#include "undoable/ListProperty-inl.h"
