//
// Created by max on 26.04.25.
//

#ifndef CHANNELS_IMPL_H
#define CHANNELS_IMPL_H
#include "ThreadsafeQueue.h"
#include "channels.h"

namespace uprotocol::core::usubscription::v3::util {

	template<typename T>
	struct ChannelBuilder;

	template<typename T>
	struct ReceiverChannelImpl : ReceiverChannel<T> {
		T receive() override;
	private:
		friend struct ChannelBuilder<T>;
		explicit ReceiverChannelImpl(std::shared_ptr<ThreadsafeQueue<T>> queue)
			: _queue(std::move(queue)) {};
		std::shared_ptr<ThreadsafeQueue<T>> _queue;
	};

	template<typename T>
	struct SenderChannelImpl : SenderChannel<T> {
		void send(const T& value) override;
	private:
		friend struct ChannelBuilder<T>;
	    explicit SenderChannelImpl(std::shared_ptr<ThreadsafeQueue<T>> queue)
	        : _queue(std::move(queue)) {};
		std::shared_ptr<ThreadsafeQueue<T>> _queue;
	};

	template<typename T>
	struct ChannelBuilder {
		std::tuple<ReceiverChannel<T>, SenderChannel<T>> build();
	};


} // namespace uprotocol::core::usubscription::v3::util

#endif //CHANNELS_IMPL_H
