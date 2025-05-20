#ifndef REQUESTBUILDER_H
#define REQUESTBUILDER_H
#include <up-cpp/utils/ProtoConverter.h>

#include <utility>
#include "up-cpp/client/usubscription/v3/RpcClientUSubscription.h"


namespace uprotocol::core::usubscription::v3 {

/**
 * @struct USubscriptionOptions
 * @brief Additional details for uSubscription service.
 *
 * Each member represents an optional parameter for the uSubscription service.
 */
struct USubscriptionOptions {
	/// Permission level of the subscription request
	std::optional<uint32_t> permission_level;
	/// TAP token for access.
	std::optional<std::string> token;
	/// Expiration time of the subscription.
	std::optional<std::chrono::system_clock::time_point> when_expire;
	/// Sample period for the subscription messages in milliseconds.
	std::optional<std::chrono::milliseconds> sample_period_ms;
	/// Details of the subscriber.
	std::optional<google::protobuf::Any> subscriber_details;
	/// Details of the subscription.
	std::optional<google::protobuf::Any> subscription_details;
};

/**
 * @brief Builds different requests using specified options.
 *
 * This struct facilitates the construction of requests based on `USubscriptionOptions`,
 * providing methods to build different requests.
 */
struct RequestBuilder {
    /**
     * @brief Constructs a RequestBuilder with the given subscription options.
     * 
     * @param options Subscription options to configure the requests. Defaults to empty options.
     */
    explicit RequestBuilder(USubscriptionOptions options = {}) 
        : options_(std::move(options)) {}

    /**
     * @brief Builds a subscription request for a given topic.
     * 
     * @param topic The `v1::UUri` representing the topic for the subscription.
     * 
     * @return A `SubscriptionRequest` configured for the specified topic.
     */
    SubscriptionRequest buildSubscriptionRequest(const v1::UUri& topic) const;

    /**
     * @brief Builds an unsubscription request for a given topic.
     * 
     * @param topic The `v1::UUri` representing the topic to unsubscribe from.
     * 
     * @return An `UnsubscribeRequest` configured for the specified topic.
     */
    UnsubscribeRequest buildUnsubscribeRequest(const v1::UUri& topic) const;

private:
    USubscriptionOptions options_; ///< Options used to configure the requests.
};


} // namespace uprotocol::core::usubscription::v3
#endif //REQUESTBUILDER_H
