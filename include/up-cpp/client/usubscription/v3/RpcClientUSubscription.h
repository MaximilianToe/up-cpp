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
#include <up-cpp/transport/UTransport.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include "up-cpp/client/usubscription/v3/USubscription.h"
#include "up-cpp/client/usubscription/v3/USubscriptionUUriBuilder.h"

namespace uprotocol::core::usubscription::v3 {
using v3::SubscriptionRequest;
using v3::UnsubscribeRequest;
using v3::Update;
using v3::uSubscription;

/// @brief Interface for uEntities to create subscriptions.
///
/// Like all L3 client APIs, the RpcClientUSubscription is a wrapper on top of
/// the L2 Communication APIs and USubscription service.
struct RpcClientUSubscription : USubscription {
	using RpcClientUSubscriptionOrStatus =
	    utils::Expected<std::unique_ptr<RpcClientUSubscription>, v1::UStatus>;
	using ListenCallback = transport::UTransport::ListenCallback;
	using ListenHandle = transport::UTransport::ListenHandle;

	template <typename Response>
	Response invokeResponse(communication::RpcClient rpc_client);

	/// @brief Subscribe to the topic
	///
	utils::Expected<SubscriptionResponse, v1::UStatus> subscribe(
	    const SubscriptionRequest& subscription_request) override;

	/// @brief Unsubscribe from the topic
	///
	utils::Expected<UnsubscribeResponse, v1::UStatus> unsubscribe(
	    const UnsubscribeRequest& subscription_request) override;

	/// @brief Constructor
	///
	/// @param transport Transport to register with.
	explicit RpcClientUSubscription(
	    std::shared_ptr<transport::UTransport> transport)
	    : transport_(std::move(transport)) {}

	/// @brief Destructor
	~RpcClientUSubscription() override = default;

private:
	// Transport
	std::shared_ptr<transport::UTransport> transport_;

	// URI info about the uSubscription service
	USubscriptionUUriBuilder uuri_builder_;
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // RPCCLIENTUSUBSCRIPTION_H