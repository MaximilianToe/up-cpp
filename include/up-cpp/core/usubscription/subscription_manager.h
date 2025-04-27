#ifndef SUBSCRIPTION_MANAGER_H
#define SUBSCRIPTION_MANAGER_H

#include <up-cpp/transport/UTransport.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include <thread>
#include <utility>
#include <variant>

#include "up-cpp/core/usubscription/configuration.h"
#include "up-cpp/core/usubscription/persistency.h"
#include "up-cpp/core/usubscription/util/channels.h"

namespace uprotocol::core::usubscription::v3 {

// Define events
struct AddSubscription {
	v1::UUri subscriber;
	v1::UUri topic;
	// Sender<int> respond_to;
};

struct RemoveSubscription {
	v1::UUri subscriber;
	v1::UUri topic;
	// Sender<int> respond_to;
};

// TODO(max) this is just a placeholder, there might be something like this in the usubscription.ph.h file
enum class TopicState {
	SUBSCRIBED,
	UNSUBSCRIBED,
	UNKNOWN
};

struct RemoteSubscriptionState {
	v1::UUri topic;
	TopicState state;
};

using SubscriptionEvent = std::variant<AddSubscription, RemoveSubscription>;
using RemoteSubscriptionEvent = std::variant<RemoteSubscriptionState>;
	using Event = std::variant<SubscriptionEvent, RemoteSubscriptionEvent>;
using UTransport = transport::UTransport;

struct SubscriptionManager {
	SubscriptionManager(std::shared_ptr<UTransport> transport,
	                     USubscriptionConfiguration  config)
	    : _transport(std::move(transport)), _config(std::move(config)){};

	void handle_message(
	    std::shared_ptr<UTransport> transport,
	    std::unique_ptr<util::ReceiverChannel<SubscriptionEvent>> subscription_receiver,
	    std::condition_variable& shutdown
	    );

	utils::Expected<SubscriptionStatus, PersistencyError> add_aubscription(
		std::shared_ptr<UTransport> transport,
		std::unique_ptr<util::ReceiverChannel<RemoteSubscriptionEvent>> remote_sub_sender,
		SubscriptionStore top_subscription_store,
		RemoteTopicStore remote_topic_store,
		v1::UUri subscriber,
		v1::UUri topic);


private:
	std::shared_ptr<UTransport> _transport;
	USubscriptionConfiguration _config;
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // SUBSCRIPTION_MANAGER_H