#ifndef SUBSCRIPTION_MANAGER_H
#define SUBSCRIPTION_MANAGER_H

#include <up-cpp/transport/UTransport.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include <variant>

#include "configuration.h"
#include "util/channels.h"

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

using SubscriptionEvent = std::variant<AddSubscription, RemoveSubscription>;
using UTransport = transport::UTransport;

struct subscription_manager {
	subscription_manager(std::shared_ptr<UTransport> transport,const USubscriptionConfiguration& config)
	:_transport(std::move(transport)), _config(std::move(config)){};

    void handle_message(
	    const UTransport& transport,
	    util::receiver_channel<SubscriptionEvent>& subscription_receiver);

	void stop_execution();

private:
	bool executing_;
	std::shared_ptr<UTransport> _transport;
	USubscriptionConfiguration _config;
};

} // namespace uprotocol::core::usubscription::v3

#endif //SUBSCRIPTION_MANAGER_H