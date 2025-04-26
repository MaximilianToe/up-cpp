//
// Created by max on 23.04.25.
//

#include "up-cpp/core/usubscription/usubscription.h"

#include <thread>

#include "up-cpp/client/usubscription/v3/Consumer.h"
#include "up-cpp/communication/RpcServer.h"
#include "up-cpp/core/usubscription/handlers/subscribe.h"
#include "up-cpp/core/usubscription/util/channels_impl.h"
#include "up-cpp/transport/UTransport.h"

namespace uprotocol::core::usubscription::v3 {

using UTransport = transport::UTransport;
using stopper_or_status = utils::Expected<USubscriptionStopper, v1::UStatus>;

void USubscriptionStopper::stop() {
	// TODO(max) dummies for now, replace with conditional variables
	running_service = false;
	running_notification_manager = false;
	running_subscription_manager = false;
}

USubscriptionService::USubscriptionService(
    std::shared_ptr<UTransport> transport, USubscriptionConfiguration config,
    v1::UPayloadFormat format, std::chrono::milliseconds ttl)
    : RpcServer(transport, std::move(format), std::move(ttl)),
      transport_(std::move(transport)),
      config_(std::move(config)){};

stopper_or_status USubscriptionService::run() {
	using USubscriptionUUriBuilder =
	    client::usubscription::v3::USubscriptionUUriBuilder;
	util::ChannelBuilder<SubscriptionEvent> builder;
	auto [subscription_receiver, subscription_sender] = builder.build();

	subscription_manager manager(transport_, config_);
	std::thread manager_thread(manager.handle_message, transport_,
	                           subscription_receiver);

	USubscriptionUUriBuilder uuri_builder;
	// what is the correct uuri here? dummy for now
	v1::UUri subscribe_uuri = uuri_builder.getServiceUriWithResourceId(0x0001);
	v1::UStatus connection_status =
	    connect(subscribe_uuri, handlers::subscribe);
	if (connection_status.code() != v1::UCode::OK) {
		return stopper_or_status(utils::Unexpected(connection_status));
	}

	return stopper_or_status(USubscriptionStopper());
}

}  // namespace uprotocol::core::usubscription::v3
