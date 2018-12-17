#pragma once


namespace undoable {

class Registry;
class Registered;
template<typename Type, typename Tag> class StaticRegistry;


class Registered {
public:
	virtual ~Registered() = default;
	virtual void Notify(void* userdata) = 0;

private:
	friend class Registry;

	Registered* next_ = nullptr;
};


class Registry {
public:
	void NotifyAll(void* userdata);
	void Add(Registered* node);

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

} // namespace undoable
