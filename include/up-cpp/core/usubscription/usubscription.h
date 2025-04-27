//
// Created by max on 23.04.25.
//

#ifndef USUBSCRIPTION_H
#define USUBSCRIPTION_H

#include <up-cpp/transport/UTransport.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include <thread>

#include "configuration.h"
#include "notification_manager.h"
#include "subscription_manager.h"
#include "up-cpp/communication/RpcServer.h"
#include "up-cpp/core/usubscription/util/channels.h"

namespace uprotocol::core::usubscription::v3 {

struct USubscriptionStopper;

using UTransport = transport::UTransport;
using StopperOrStatus = utils::Expected<USubscriptionStopper, v1::UStatus>;

struct USubscriptionService : communication::RpcServer {
	explicit USubscriptionService(
	    std::shared_ptr<transport::UTransport> transport,
	    USubscriptionConfiguration config, v1::UPayloadFormat format = {},
	    std::chrono::milliseconds ttl = {});

	StopperOrStatus run();

	v1::UStatus register_handlers(
	    std::unique_ptr<util::SenderChannel<SubscriptionEvent>>
	        subscription_sender,
	    std::unique_ptr<util::SenderChannel<NotificationEvent>>
	        notification_channel);

	// TODO(max) maybe return a stopper struct instead
	void stop();

	struct USubscriptionStopper {
		explicit USubscriptionStopper() = default;
		// 	    : service_(std::make_unique<USubscriptionService>(this)) {}
		//
		// 	void stop() {service_->stop();}
		// 	~USubscriptionStopper(){stop();}
		//
		// private:
		// 	std::unique_ptr<USubscriptionService> service_;
	};

private:
	std::atomic<bool> stop_{false};
	std::thread subscription_manager_thread_;
	std::thread notification_manager_thread_;
	std::shared_ptr<UTransport> transport_;
	USubscriptionConfiguration config_;
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // USUBSCRIPTION_H
