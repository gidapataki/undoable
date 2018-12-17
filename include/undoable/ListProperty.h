#pragma once
#include <iterator>
#include <type_traits>
#include <vector>
#include "undoable/Property.h"
#include "undoable/Command.h"


namespace undoable {

struct tag_Unlinker;
class PropertyOwner;

template<typename Type, typename Tag> class ListNode;
template<typename Type, typename Tag> class ListProperty;
template<typename Type, typename Tag> class ListIterator;


// --

class Registry;
class Registered;


class Registered {
public:
	virtual ~Registered() {}
	virtual void Notify(void* userdata) {}

private:
	friend class Registry;

	Registered* next_ = nullptr;
};


class Registry {
public:
	void NotifyAll(void* userdata) {
		for (auto* p = first_; p; p = p->next_) {
			p->Notify(userdata);
		}
	}

	void Add(Registered* node) {
		if (!last_) {
			first_ = last_ = node;
		} else {
			last_->next_ = node;
			last_ = node;
		}
	}

private:
	Registered* first_ = nullptr;
	Registered* last_ = nullptr;
};


template<typename Type, typename Tag>
class StaticRegistry
	: public Registry
{
public:
	static StaticRegistry& Get() {
		static StaticRegistry instance;
		return instance;
	}

private:
	StaticRegistry() = default;
};


template<typename Type, typename Tag>
class ListNode {
public:
	ListNode();
	~ListNode();
	ListNode(const ListNode&) = delete;
	ListNode(ListNode&&) = delete;
	ListNode& operator=(const ListNode&) = delete;
	ListNode& operator=(ListNode&&) = delete;

	void Unlink();
	bool IsLinked() const;
	Type* Object();
	const Type* Object() const;

private:
	friend class ListProperty<Type, Tag>;
	friend class ListIterator<Type, Tag>;
	friend class ListIterator<const Type, Tag>;
	using ListProperty = ListProperty<Type, Tag>;

	class Relink : public Command {
	public:
		Relink(ListNode* node, ListNode* next, ListProperty* parent);
		virtual void Apply(bool reverse) override;

	private:
		ListNode* node_;
		ListNode* next_;
		ListProperty* parent_;
	};

	class Unlinker : public Registered {
	public:
		Unlinker(Registry* registry) {
			registry->Add(this);
		}

		virtual void Notify(void* userdata) {
			auto* obj = reinterpret_cast<Type*>(userdata);
			static_cast<ListNode*>(obj)->Unlink();
		}
	};

	static void Link(ListNode* u, ListNode* v);
	static ListNode& Next(ListNode& node);
	static ListNode& Prev(ListNode& node);
	static ListNode* Next(ListNode* node);
	static ListNode* Prev(ListNode* node);

	ListProperty* parent_;
	ListNode* next_;
	ListNode* prev_;
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


template<typename Type, typename Tag>
class ListProperty
	: public Property {
public:
	friend class ListNode<Type, Tag>;

	using ListNode = ListNode<Type, Tag>;
	using iterator = ListIterator<Type, Tag>;
	using const_iterator = ListIterator<const Type, Tag>;

	ListProperty(PropertyOwner* owner);
	~ListProperty();

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

	ListNode head_;
	bool owning_ = false;
};


template<typename Type, typename Tag>
class OwningListProperty
	: public ListProperty<Type, Tag>
{
public:
	OwningListProperty(PropertyOwner* owner);
};


} // namespace

#include "undoable/ListProperty-inl.h"
