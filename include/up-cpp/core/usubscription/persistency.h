//
// Created by max on 27.04.25.
//

#ifndef PERSISTENCY_H
#define PERSISTENCY_H
#include "configuration.h"
#include "up-cpp/datamodel/serializer/UUri.h"
#include "up-cpp/utils/Expected.h"

namespace uprotocol::core::usubscription::v3 {

struct PersistencyError {};

struct SubscriptionStore {
	explicit SubscriptionStore(USubscriptionConfiguration config);

	utils::Expected<bool, PersistencyError> add_subscription(
	    const v1::UUri& subscriber, const v1::UUri& topic);

private:
	std::unordered_map<std::string, std::unordered_set<std::string>>
	    persistency_;
};

struct RemoteTopicStore {
	explicit RemoteTopicStore(USubscriptionConfiguration config);
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // PERSISTENCY_H
