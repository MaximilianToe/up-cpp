//
// Created by max on 26.04.25.
//

#ifndef CHANNELS_IMPL_H
#define CHANNELS_IMPL_H
#include "channels.h"
#include "threadsafe_queue.h"

namespace uprotocol::core::usubscription::v3::util {

template <typename T>
struct ChannelBuilder;

template <typename T>
struct ReceiverChannelImpl : ReceiverChannel<T> {
	T receive() override;

private:
	friend struct ChannelBuilder<T>;
	explicit ReceiverChannelImpl(std::shared_ptr<threadsafe_queue<T>> queue)
	    : _queue(std::move(queue)){};
	std::shared_ptr<threadsafe_queue<T>> _queue;
};

template <typename T>
struct SenderChannelImpl : SenderChannel<T> {
	void send(const T& value) override;

private:
	friend struct ChannelBuilder<T>;
	explicit SenderChannelImpl(std::shared_ptr<threadsafe_queue<T>> queue)
	    : _queue(std::move(queue)){};
	std::shared_ptr<threadsafe_queue<T>> _queue;
};

template <typename T>
struct ChannelBuilder {
	std::tuple<std::unique_ptr<ReceiverChannel<T>>,
	           std::unique_ptr<SenderChannel<T>>>
	build();
};

}  // namespace uprotocol::core::usubscription::v3::util

#endif  // CHANNELS_IMPL_H
