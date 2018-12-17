#pragma once
#include <type_traits>
#include "undoable/Tracked.h"
#include "undoable/History.h"
#include "intrusive/List.h"

namespace undoable {

class Factory {
public:
	Factory() = default;
	~Factory();

	History& GetHistory();

	template<typename T, typename... Args>
	T& Create(Args&&... args) {
		static_assert(
			std::is_base_of<Tracked, T>::value,
			"Type is not derived from `Tracked`");

		T* obj = new T(args...);
		Tracked* tracked = static_cast<Tracked*>(obj);

		objects_.LinkBack(*tracked);
		tracked->Init(&history_);
		return *obj;
	}

private:
	History history_;
	intrusive::List<Tracked> objects_;
};

} // namespace undoable
