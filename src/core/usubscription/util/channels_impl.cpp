//
// Created by max on 26.04.25.
//

#include "up-cpp/core/usubscription/util/channels_impl.h"

namespace uprotocol::core::usubscription::v3::util {

	template<typename T>
	T ReceiverChannelImpl<T>::receive() override {
		return _queue->receive();
	}

	template<typename T>
	void SenderChannelImpl<T>::send(const T& value) override {
		_queue->send(value);
	}

	template<typename T>
	std::tuple<ReceiverChannel<T>,SenderChannel<T>> ChannelBuilder<T>::build() {
		auto queue = std::make_shared<ThreadsafeQueue<T>>();
		return std::make_tuple(ReceiverChannelImpl<T>(queue)
			,SenderChannelImpl<T>(queue));
	}

} // namespace uprotocol::core::usubscription::v3::util
