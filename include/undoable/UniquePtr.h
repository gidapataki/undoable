#pragma once
#include <memory>

namespace undoable {

template<typename T> using UniquePtr = std::unique_ptr<T>;
template<typename T, typename... Args>
UniquePtr<T> MakeUnique(Args&&... args) {
	return std::make_unique<T>(args...);
}

} // namespace undoable
