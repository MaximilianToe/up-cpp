#include "up-cpp/core/usubscription/subscription_manager.h"
#include "up-cpp/core/usubscription/persistency.h"

namespace uprotocol::core::usubscription::v3 {
using UTransport = transport::UTransport;
using StatusOrError = utils::Expected<SubscriptionStatus,PersistencyError>;

	[[noreturn]] void handle_message(
	    const UTransport& transport,
	    std::unique_ptr<util::ReceiverChannel<SubscriptionEvent>> subscription_receiver,
	    std::condition_variable& shutdown
	    ){
	while (true) {
		auto event = subscription_receiver->receive();
	}
};


StatusOrError add_aubscription(
	std::shared_ptr<UTransport> transport,
	std::unique_ptr<util::ReceiverChannel<RemoteSubscriptionEvent>> remote_sub_sender,
	SubscriptionStore top_subscription_store,
	RemoteTopicStore remote_topic_store,
	v1::UUri subscriber,
	v1::UUri topic) {
	//dummy
	return StatusOrError(SubscriptionStatus());
};

}  // namespace uprotocol::core::usubscription::v3