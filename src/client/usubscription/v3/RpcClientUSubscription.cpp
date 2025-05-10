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

#include <up-cpp/client/usubscription/v3/RpcClientUSubscription.h>
#include <up-cpp/client/usubscription/v3/USubscription.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <spdlog/spdlog.h>

#include <utility>

#include "up-cpp/communication/RpcClient.h"
#include "up-cpp/transport/UTransport.h"

constexpr uint16_t RESOURCE_ID_SUBSCRIBE = 0x0001;
// TODO(lennart) see default_call_options() for the request in Rust
constexpr auto SUBSCRIPTION_REQUEST_TTL =
    std::chrono::milliseconds(0x0800);  // TODO(lennart) change time
auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4;  // MUST be >= 4

namespace uprotocol::core::usubscription::v3 {

RpcClientUSubscription::RpcClientUSubscription(
    std::shared_ptr<transport::UTransport> transport,
    RpcClientUSubscriptionOptions rpc_client_usubscription_options)
    : transport_(std::move(transport)),
      rpc_client_usubscription_options_(
          std::move(rpc_client_usubscription_options)) {
	uuri_builder_ = USubscriptionUUriBuilder();
}

SubscriptionRequest RpcClientUSubscription::buildSubscriptionRequest(const v1::UUri& subscription_topic) {
	auto attributes = utils::ProtoConverter::BuildSubscribeAttributes(
	    rpc_client_usubscription_options_.when_expire,
	    rpc_client_usubscription_options_.subscription_details,
	    rpc_client_usubscription_options_.sample_period_ms);

	auto subscription_request = utils::ProtoConverter::BuildSubscriptionRequest(
	    subscription_topic, attributes);
	return subscription_request;
}

RpcClientUSubscription::ResponseOrStatus<SubscriptionResponse>
RpcClientUSubscription::subscribe(
    const SubscriptionRequest& subscription_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(
	        RESOURCE_ID_SUBSCRIBE),
	    priority, SUBSCRIPTION_REQUEST_TTL);

	datamodel::builder::Payload payload(subscription_request);

	auto invoke_future = rpc_client.invokeMethod(std::move(payload));

	auto message_or_status = invoke_future.get();

	if (!message_or_status.has_value()) {
		return ResponseOrStatus<SubscriptionResponse>(
		    utils::Unexpected<v1::UStatus>(std::move(message_or_status.error())));
	}

	SubscriptionResponse subscription_response;
	subscription_response.ParseFromString(message_or_status.value().payload());

	if (subscription_response.topic().SerializeAsString() !=
	    subscription_request.topic().SerializeAsString()) {
		return ResponseOrStatus<SubscriptionResponse>(
			utils::Unexpected<v1::UStatus>(std::move(message_or_status.error())));
	}

	return ResponseOrStatus<SubscriptionResponse>(std::move(subscription_response));
}

}  // namespace uprotocol::core::usubscription::v3
