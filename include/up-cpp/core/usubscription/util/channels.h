//
// Created by max on 26.04.25.
//

#ifndef CHANNELS_H
#define CHANNELS_H
namespace uprotocol::core::usubscription::v3::util {
// interface that a class needs to implement to act as a receiver channel for
// IPC
template <typename T>
struct ReceiverChannel {
	virtual ~ReceiverChannel() = default;
	virtual T receive();
};

// interface that a class needs to implement to act as a sender channel for IPC
template <typename T>
struct SenderChannel {
	virtual ~SenderChannel() = default;
	virtual void send(const T& value);
};
}  // namespace uprotocol::core::usubscription::v3::util

#endif  // CHANNELS_H
