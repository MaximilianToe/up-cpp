// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RPCCLIENTUSUBSCRIPTION_H
#define RPCCLIENTUSUBSCRIPTION_H

#include <up-cpp/communication/NotificationSink.h>
#include <up-cpp/communication/RpcClient.h>
#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/utils/ProtoConverter.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <up-cpp/transport/UTransport.h>

#include "up-cpp/client/usubscription/v3/USubscription.h"
#include "up-cpp/client/usubscription/v3/USubscriptionUUriBuilder.h"

namespace uprotocol::core::usubscription::v3 {
using v3::SubscriptionRequest;
using v3::UnsubscribeRequest;
using v3::Update;
using v3::uSubscription;

/**
 * @struct RpcClientUSubscriptionOptions
 * @brief Additional details for uSubscription service.
 *
 * Each member represents an optional parameter for the uSubscription service.
 */
struct RpcClientUSubscriptionOptions {
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


/// @brief Interface for uEntities to create subscriptions.
///
/// Like all L3 client APIs, the RpcClientUSubscription is a wrapper on top of
/// the L2 Communication APIs and USubscription service.
struct RpcClientUSubscription : USubscription {
	using RpcClientUSubscriptionOrStatus =
	    utils::Expected<std::unique_ptr<RpcClientUSubscription>, v1::UStatus>;
	using ListenCallback = transport::UTransport::ListenCallback;
	using ListenHandle = transport::UTransport::ListenHandle;

	/// @brief Create a subscription
	///
	/// @param transport Transport to register with.
	/// @param subscription_topic Topic to subscribe to.
	/// @param callback Function that is called when publish message is
	/// received.
	/// @param priority Priority of the subscription request.
	/// @param subscribe_request_ttl Time to live for the subscription request.
	/// @param rpc_client_usubscription_options Additional details for
	/// uSubscription service.
	// [[nodiscard]] static RpcClientUSubscriptionOrStatus create(
	//     std::shared_ptr<transport::UTransport> transport,
	//     const v1::UUri& subscription_topic, ListenCallback&& callback,
	//     RpcClientUSubscriptionOptions rpc_client_usubscription_options);

	/// @brief Subscribe to the topic
	///
	utils::Expected<SubscriptionResponse, v1::UStatus> subscribe(
	    const SubscriptionRequest& subscription_request) override;

	/// @brief Destructor
	~RpcClientUSubscription() override = default;

	/// This section for test code only delete later

	// protected:
	/// @brief Constructor
	///
	/// @param transport Transport to register with.
	/// @param subscriber_details Additional details about the subscriber.
	explicit RpcClientUSubscription(
	    std::shared_ptr<transport::UTransport> transport,
	    RpcClientUSubscriptionOptions rpc_client_usubscription_options = {});

private:
	// Transport
	std::shared_ptr<transport::UTransport> transport_;

	// Additional details about uSubscription service
	RpcClientUSubscriptionOptions rpc_client_usubscription_options_;

	// URI info about the uSubscription service
	USubscriptionUUriBuilder uuri_builder_;

	// Allow the protected constructor for this class to be used in make_unique
	// inside of create()
	// friend std::unique_ptr<RpcClientUSubscription> std::make_unique<
	//     RpcClientUSubscription, std::shared_ptr<transport::UTransport>,
	//     const v1::UUri, RpcClientUSubscriptionOptions>(
	//     std::shared_ptr<uprotocol::transport::UTransport>&&, const v1::UUri&&,
	//     RpcClientUSubscriptionOptions&&);

public:
	/// @brief Build SubscriptionRequest for subscription request
	SubscriptionRequest buildSubscriptionRequest(const v1::UUri& subscription_topic);
	//
	// /// @brief  Build UnsubscriptionRequest for unsubscription request
	// UnsubscribeRequest buildUnsubscriptionRequest();
	//
	// /// @brief Create a notification sink to receive subscription updates
	// v1::UStatus createNotificationSink();
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // RPCCLIENTUSUBSCRIPTION_H