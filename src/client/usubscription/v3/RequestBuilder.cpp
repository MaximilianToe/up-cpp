#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <iostream>
#include "up-cpp/client/usubscription/v3/RequestBuilder.h"

namespace uprotocol::core::usubscription::v3 {


SubscriptionRequest RequestBuilder::buildSubscriptionRequest(
	const v1::UUri& topic) const {
	auto attributes = utils::ProtoConverter::BuildSubscribeAttributes(
		options_.when_expire,
		options_.subscription_details,
		options_.sample_period_ms);
	if (options_.permission_level.has_value()) {
		std::cout << "PERMISSION LEVEL:" << options_.permission_level.value()<<std::endl;
	} else {
		std::cout << "no when_expire" << std::endl;
	}

	return utils::ProtoConverter::BuildSubscriptionRequest(
		topic, attributes);
}

UnsubscribeRequest RequestBuilder::buildUnsubscribeRequest(
    const v1::UUri& topic) {
	// auto attributes = utils::ProtoConverter::BuildSubscribeAttributes(
	// 	options_.when_expire,
	// 	options_.subscription_details,
	// 	options_.sample_period_ms);

	return utils::ProtoConverter::BuildUnSubscribeRequest(
		topic);
}


} // namespace uprotocol::core::usubscription::v3
