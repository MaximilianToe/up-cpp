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
#include "up-cpp/utils/Expected.h"
#include "up-cpp/utils/ProtoConverter.h"

/// The uEntity (type) identifier of the uSubscription service.
constexpr uint32_t USUBSCRIPTION_TYPE_ID = 0x00000000;
/// The (latest) major version of the uSubscription service.
constexpr uint8_t USUBSCRIPTION_VERSION_MAJOR = 0x03;
/// The resource identifier of uSubscription's _subscribe_ operation.
constexpr uint16_t RESOURCE_ID_SUBSCRIBE = 0x0001;
/// The resource identifier of uSubscription's _unsubscribe_ operation.
constexpr uint16_t RESOURCE_ID_UNSUBSCRIBE = 0x0002;
/// The resource identifier of uSubscription's _fetch subscriptions_ operation.
constexpr uint16_t RESOURCE_ID_FETCH_SUBSCRIPTIONS = 0x0003;
/// The resource identifier of uSubscription's _register for notifications_ operation.
constexpr uint16_t RESOURCE_ID_REGISTER_FOR_NOTIFICATIONS = 0x0006;
/// The resource identifier of uSubscription's _unregister for notifications_ operation.
constexpr uint16_t RESOURCE_ID_UNREGISTER_FOR_NOTIFICATIONS = 0x0007;
/// The resource identifier of uSubscription's _fetch subscribers_ operation.
constexpr uint16_t RESOURCE_ID_FETCH_SUBSCRIBERS = 0x0008;

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

	auto response_or_status =
	    rpc_client.invokeProtoMethod<SubscriptionResponse>(subscription_request);

	if (!response_or_status.has_value()) {
		return utils::Expected<SubscriptionResponse, v1::UStatus>(
		    utils::Unexpected(response_or_status.error()));
	}
	auto subscription_response = response_or_status.value();

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

	return rpc_client.invokeProtoMethod<UnsubscribeResponse>(unsubscribe_request);
}

RpcClientUSubscription::ResponseOrStatus<FetchSubscriptionsResponse>
RpcClientUSubscription::fetch_subscriptions(
    const FetchSubscriptionsRequest& fetch_subscribers_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_FETCH_SUBSCRIPTIONS),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	return rpc_client.invokeProtoMethod<FetchSubscriptionsResponse>(fetch_subscribers_request);
}

RpcClientUSubscription::ResponseOrStatus<FetchSubscribersResponse>
RpcClientUSubscription::fetch_subscribers(
    const FetchSubscribersRequest& fetch_subscribers_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_FETCH_SUBSCRIBERS),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	return rpc_client.invokeProtoMethod<FetchSubscribersResponse>(fetch_subscribers_request);
}

RpcClientUSubscription::ResponseOrStatus<NotificationsResponse>
RpcClientUSubscription::register_for_notifications(
    const NotificationsRequest& register_notifications_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_REGISTER_FOR_NOTIFICATIONS),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	return rpc_client.invokeProtoMethod<NotificationsResponse>(register_notifications_request);
}

RpcClientUSubscription::ResponseOrStatus<NotificationsResponse>
RpcClientUSubscription::unregister_for_notifications(
    const NotificationsRequest& unregister_notifications_request) {
	communication::RpcClient rpc_client(
	    transport_,
	    uuri_builder_.getServiceUriWithResourceId(RESOURCE_ID_UNREGISTER_FOR_NOTIFICATIONS),
	    priority, USUBSCRIPTION_REQUEST_TTL);

	return rpc_client.invokeProtoMethod<NotificationsResponse>(unregister_notifications_request);
}

}  // namespace uprotocol::core::usubscription::v3
