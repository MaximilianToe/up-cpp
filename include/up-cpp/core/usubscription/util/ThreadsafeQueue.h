//
// Created by max on 26.04.25.
//

#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H
#include <condition_variable>
#include <mutex>
#include <queue>
namespace uprotocol::core::usubscription::util {

	template<typename T>
	struct ThreadsafeQueue {

		void send(const T& value);

		T receive();

	private:
		std::queue<T> _queue;
		std::mutex _mutex;
		std::condition_variable _condition;
	};

} // uprotocol::core::usubscription::util

#endif //THREADSAFEQUEUE_H
