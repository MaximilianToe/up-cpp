//
// Created by max on 26.04.25.
//

#include "up-cpp/core/usubscription/util/channels_impl.h"

namespace uprotocol::core::usubscription::v3::util {

template <typename T>
T ReceiverChannelImpl<T>::receive() override {
	return _queue->receive();
}

template <typename T>
void SenderChannelImpl<T>::send(const T& value) override {
	_queue->send(value);
}

template <typename T>
std::tuple<std::unique_ptr<ReceiverChannel<T>>,
           std::unique_ptr<SenderChannel<T>>>
ChannelBuilder<T>::build() {
	auto queue = std::make_shared<ThreadsafeQueue<T>>();
	auto receiver_channel = std::make_unique<ReceiverChannelImpl<T>>(queue);
	auto sender_channel = std::make_unique<SenderChannelImpl<T>>(queue);
	return std::make_tuple(receiver_channel, sender_channel);
}

}  // namespace uprotocol::core::usubscription::v3::util
