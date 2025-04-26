//
// Created by max on 23.04.25.
//

#ifndef USUBSCRIPTION_H
#define USUBSCRIPTION_H

#include <up-cpp/transport/UTransport.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include "configuration.h"
#include "up-cpp/communication/RpcServer.h"

namespace uprotocol::core::usubscription::v3 {

struct USubscriptionStopper {
	void stop();
	~USubscriptionStopper() { stop(); }
	// TODO(max) handlers to stop services, dummies for now
	bool running_service;
	bool running_subscription_manager;
	bool running_notification_manager;
};
using UTransport = transport::UTransport;
using stopper_or_status = utils::Expected<USubscriptionStopper, v1::UStatus>;

struct USubscriptionService : communication::RpcServer {
	explicit USubscriptionService(
	    std::shared_ptr<transport::UTransport> transport,
	    USubscriptionConfiguration config, v1::UPayloadFormat format = {},
	    std::chrono::milliseconds ttl = {});

	// TODO(max) make async
	utils::Expected<USubscriptionStopper, v1::UStatus> run();

private:
	std::shared_ptr<UTransport> transport_;
	USubscriptionConfiguration config_;
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // USUBSCRIPTION_H
