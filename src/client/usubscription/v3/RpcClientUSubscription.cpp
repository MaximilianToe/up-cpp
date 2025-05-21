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

#include <spdlog/spdlog.h>
#include <up-cpp/client/usubscription/v3/RpcClientUSubscription.h>
#include <up-cpp/client/usubscription/v3/USubscription.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <utility>

#include "up-cpp/communication/RpcClient.h"
#include "up-cpp/utils/ProtoConverter.h"

constexpr uint16_t RESOURCE_ID_SUBSCRIBE = 0x0001;
constexpr uint16_t RESOURCE_ID_UNSUBSCRIBE = 0x0002;
// TODO(lennart) see default_call_options() for the request in Rust
constexpr auto USUBSCRIPTION_REQUEST_TTL =
    std::chrono::milliseconds(0x0800);  // TODO(lennart) change time
auto priority = uprotocol::v1::UPriority::UPRIORITY_CS4;  // MUST be >= 4

namespace uprotocol::core::usubscription::v3 {

template <typename Response>
Response RpcClientUSubscription::invokeResponse(
    communication::RpcClient rpc_client) {}

RpcClientUSubscription::ResponseOrStatus<SubscriptionResponse>
RpcClientUSubscription::subscribe(
    const SubscriptionRequest& subscription_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_SUBSCRIBE),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	auto payload_or_status =
	    utils::ProtoConverter::protoToPayload(subscription_request);

	if (!payload_or_status.has_value()) {
		return ResponseOrStatus<SubscriptionResponse>(
		    utils::Unexpected<v1::UStatus>(payload_or_status.error()));
	}
	datamodel::builder::Payload payload(payload_or_status.value());

	auto message_or_status = rpc_client.invokeMethod(std::move(payload)).get();

	if (!message_or_status.has_value()) {
		return ResponseOrStatus<SubscriptionResponse>(
		    utils::Unexpected<v1::UStatus>(message_or_status.error()));
	}

	spdlog::debug("response UMessage: {}",
	              message_or_status.value().DebugString());
	SubscriptionResponse subscription_response;

	auto response_or_status =
	    utils::ProtoConverter::extractFromProtobuf<SubscriptionResponse>(
	        message_or_status.value());

	if (!response_or_status.has_value()) {
		spdlog::error(
		    "subscribe: Error when extracting response from protobuf.");
		return ResponseOrStatus<SubscriptionResponse>(
		    utils::Unexpected<v1::UStatus>(response_or_status.error()));
	}

	subscription_response = response_or_status.value();

	spdlog::debug("subscribe: response: {}",
	              subscription_response.DebugString());

	if (subscription_response.topic().SerializeAsString() !=
	    subscription_request.topic().SerializeAsString()) {
		v1::UStatus status;
		status.set_code(v1::UCode::INTERNAL);
		status.set_message("subscribe: topics do not match.");
		return ResponseOrStatus<SubscriptionResponse>(
		    utils::Unexpected<v1::UStatus>(status));
	}

	return ResponseOrStatus<SubscriptionResponse>(
	    std::move(subscription_response));
}

RpcClientUSubscription::ResponseOrStatus<UnsubscribeResponse>
RpcClientUSubscription::unsubscribe(
    const UnsubscribeRequest& unsubscribe_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_UNSUBSCRIBE),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	auto payload_or_status =
	    utils::ProtoConverter::protoToPayload(unsubscribe_request);

	if (!payload_or_status.has_value()) {
		return ResponseOrStatus<UnsubscribeResponse>(
		    utils::Unexpected<v1::UStatus>(payload_or_status.error()));
	}
	datamodel::builder::Payload payload(payload_or_status.value());

	auto message_or_status = rpc_client.invokeMethod(std::move(payload)).get();

	if (!message_or_status.has_value()) {
		return ResponseOrStatus<UnsubscribeResponse>(
		    utils::Unexpected<v1::UStatus>(message_or_status.error()));
	}

	spdlog::debug("response UMessage: {}",
	              message_or_status.value().DebugString());
	UnsubscribeResponse unsubscribe_response;

	auto response_or_status =
	    utils::ProtoConverter::extractFromProtobuf<UnsubscribeResponse>(
	        message_or_status.value());

	if (!response_or_status.has_value()) {
		spdlog::error(
		    "unsubscribe: Error when extracting response from protobuf.");
		return ResponseOrStatus<UnsubscribeResponse>(
		    utils::Unexpected<v1::UStatus>(response_or_status.error()));
	}

	unsubscribe_response = response_or_status.value();

	spdlog::debug("unsubscribe: response: {}",
	              unsubscribe_response.DebugString());

	return ResponseOrStatus<UnsubscribeResponse>(
	    std::move(unsubscribe_response));
}

}  // namespace uprotocol::core::usubscription::v3
