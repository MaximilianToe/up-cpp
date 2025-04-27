//
// Created by max on 27.04.25.
//

#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H
#include "configuration.h"
#include "up-cpp/transport/UTransport.h"
#include "util/channels.h"

namespace uprotocol::core::usubscription::v3 {

using UTransport = transport::UTransport;

struct NotificationEvent {};

struct NotificationManager {
	explicit NotificationManager(std::shared_ptr<UTransport> transport,
	                             USubscriptionConfiguration config)
	    : transport_(std::move(transport)), config_(std::move(config)){};

	void handle_message(
	    std::shared_ptr<UTransport> transport,
	    std::unique_ptr<util::ReceiverChannel<NotificationEvent>>
	        notification_receiver,
	    std::condition_variable& shutdown);

private:
	std::shared_ptr<UTransport> transport_;
	USubscriptionConfiguration config_;
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // NOTIFICATION_MANAGER_H
