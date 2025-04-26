#include "up-cpp/core/usubscription/subscription_manager.h"

namespace uprotocol::core::usubscription::v3 {

void subscription_manager::handle_message(
    const transport::UTransport& transport,
    util::ReceiverChannel<SubscriptionEvent>& subscription_receiver) {
	executing_ = true;
	while (executing_) {
		auto event = subscription_receiver.receive();
	}
};

void subscription_manager::stop_execution() { executing_ = false; }

}  // namespace uprotocol::core::usubscription::v3