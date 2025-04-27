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
using StopperOrStatus = utils::Expected<USubscriptionStopper, v1::UStatus>;

USubscriptionService::USubscriptionService(
    std::shared_ptr<UTransport> transport, USubscriptionConfiguration config,
    v1::UPayloadFormat format, std::chrono::milliseconds ttl)
    : RpcServer(transport, format, ttl),
      transport_(std::move(transport)),
      config_(std::move(config)){};

StopperOrStatus USubscriptionService::run() {
	using USubscriptionUUriBuilder =
	    client::usubscription::v3::USubscriptionUUriBuilder;

	util::ChannelBuilder<SubscriptionEvent> channel_builder;
	auto [subscription_receiver, subscription_sender] = channel_builder.build();

	const SubscriptionManager subscription_manager(transport_, config_);
	subscription_manager_thread_ = std::thread(subscription_manager.handle_message, transport_,
	                           subscription_receiver);

	auto [notification_receiver, notification_sender] = channel_builder.build();

	USubscriptionUUriBuilder uuri_builder;
	// what is the correct uuri here? dummy for now
	v1::UUri subscribe_uuri = uuri_builder.getServiceUriWithResourceId(0x0001);
	v1::UStatus connection_status =
	    connect(subscribe_uuri, handlers::subscribe);
	if (connection_status.code() != v1::UCode::OK) {
		return StopperOrStatus(utils::Unexpected(connection_status));
	}

	return StopperOrStatus(USubscriptionStopper());
}

void USubscriptionService::stop() {
	stop_ = true;
	subscription_manager_thread_.join();
	notification_manager_thread_.join();
}

}  // namespace uprotocol::core::usubscription::v3
