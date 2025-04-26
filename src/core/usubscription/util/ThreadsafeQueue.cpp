//
// Created by max on 26.04.25.
//

#include "up-cpp/core/usubscription/util/ThreadsafeQueue.h"

namespace uprotocol::core::usubscription::v3::util {
template <typename T>
void ThreadsafeQueue<T>::send(const T& value) {
	std::lock_guard<std::mutex> lock(_mutex);
	_queue.push(value);
	_condition.notify_one();
}

template <typename T>
T ThreadsafeQueue<T>::receive() {
	std::lock_guard<std::mutex> lock(_mutex);
	_condition.wait([this] { return !this._queue.empty(); }, lock);
	T value = _queue.front();
	_queue.pop();
	return value;
}

}  // namespace uprotocol::core::usubscription::v3::util
