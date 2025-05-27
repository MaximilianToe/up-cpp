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

#include <google/protobuf/any.pb.h>
#include <gtest/gtest.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include <chrono>
#include <iostream>
#include <optional>
#include <ostream>

#include "up-cpp/client/usubscription/v3/RequestBuilder.h"

constexpr uint32_t SOURCE_UE_ID = 0x00011101;
constexpr uint32_t SOURCE_UE_VERSION_MAJOR = 0xF8;
constexpr uint32_t SOURCE_RESOURCE_ID = 0x8101;

namespace uprotocol::core::usubscription::v3 {

class RequestBuilderTest : public ::testing::Test {
private:
	v1::UUri source_;
	v1::UUri wrong_topic_;

protected:
	USubscriptionOptions options_;
	const v1::UUri& getSource() const { return source_; }
	const v1::UUri& getWrongTopic() const { return wrong_topic_; }

	void SetUp() override {
		// Create a UUri object for testing
		source_.set_authority_name("10.0.0.1");
		source_.set_ue_id(SOURCE_UE_ID);
		source_.set_ue_version_major(SOURCE_UE_VERSION_MAJOR);
		source_.set_resource_id(SOURCE_RESOURCE_ID);

		wrong_topic_.set_authority_name("10.0.0.2");  // random different authority
		wrong_topic_.set_ue_id(SOURCE_UE_ID);
		wrong_topic_.set_ue_version_major(SOURCE_UE_VERSION_MAJOR);
		wrong_topic_.set_resource_id(SOURCE_RESOURCE_ID);

		options_.permission_level = 2;
		options_.token = "sample_token";
		options_.when_expire =
		    std::chrono::system_clock::now() + std::chrono::milliseconds(1000);
		options_.sample_period_ms = std::chrono::milliseconds(1000);
		options_.subscriber_details = google::protobuf::Any();
		options_.subscription_details = google::protobuf::Any();
	}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	RequestBuilderTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

public:
	~RequestBuilderTest() override = default;
};

TEST_F(RequestBuilderTest, BuildSubscriptionRequestWithOptions) {
	v1::UUri topic = getSource();
	RequestBuilder builder(options_);

	SubscriptionRequest request = builder.buildSubscriptionRequest(topic);

	// Verify the attributes in the request
	EXPECT_TRUE(request.has_topic());
	EXPECT_TRUE(request.has_attributes());
	EXPECT_EQ(request.topic().SerializeAsString(), topic.SerializeAsString());
	EXPECT_NE(request.topic().SerializeAsString(),
	          getWrongTopic().SerializeAsString());
	EXPECT_EQ(request.GetTypeName(), "uprotocol.core.usubscription.v3.SubscriptionRequest");
}

TEST_F(RequestBuilderTest, BuildUnsubscribeRequest) {
    v1::UUri topic = getSource();
    RequestBuilder builder(options_);

    UnsubscribeRequest request = builder.buildUnsubscribeRequest(topic);

    // Verify the attributes in the request
    EXPECT_TRUE(request.has_topic());
    EXPECT_EQ(request.topic().SerializeAsString(), topic.SerializeAsString());
	EXPECT_NE(request.topic().SerializeAsString(), getWrongTopic().SerializeAsString());
    EXPECT_EQ(request.GetTypeName(), "uprotocol.core.usubscription.v3.UnsubscribeRequest");
}

TEST_F(RequestBuilderTest, BuildFetchSubscriptionsRequestWithTopic) {
    v1::UUri topic = getSource();
	RequestBuilder builder(options_);

    FetchSubscriptionsRequest request = builder.buildFetchSubscriptionsRequest(topic);

    // Verify the attributes in the request
    EXPECT_TRUE(request.has_topic());
	EXPECT_EQ(request.topic().SerializeAsString(), topic.SerializeAsString());
	EXPECT_NE(request.topic().SerializeAsString(), getWrongTopic().SerializeAsString());
    EXPECT_EQ(request.GetTypeName(), "uprotocol.core.usubscription.v3.FetchSubscriptionsRequest");
}

TEST_F(RequestBuilderTest, BuildFetchSubscriptionsRequestWithSubscriberInfo) {
    SubscriberInfo subscriber;
	RequestBuilder builder(options_);

    FetchSubscriptionsRequest request = builder.buildFetchSubscriptionsRequest(subscriber);

    // Verify the attributes in the request
    EXPECT_FALSE(request.has_topic());
    EXPECT_EQ(request.GetTypeName(), "uprotocol.core.usubscription.v3.FetchSubscriptionsRequest");
}

TEST_F(RequestBuilderTest, BuildFetchSubscribersRequest) {
    v1::UUri topic = getSource();
    RequestBuilder builder(options_);

    FetchSubscribersRequest request = builder.buildFetchSubscribersRequest(topic);

    // Verify the attributes in the request
    EXPECT_TRUE(request.has_topic());
    EXPECT_EQ(request.topic().SerializeAsString(), topic.SerializeAsString());
	EXPECT_NE(request.topic().SerializeAsString(), getWrongTopic().SerializeAsString());
    EXPECT_EQ(request.GetTypeName(), "uprotocol.core.usubscription.v3.FetchSubscribersRequest");
}

TEST_F(RequestBuilderTest, BuildNotificationsRequest) {
    v1::UUri topic = getSource();
    RequestBuilder builder(options_);

    NotificationsRequest request = builder.buildNotificationsRequest(topic);

    // Verify the attributes in the request
    EXPECT_TRUE(request.has_topic());
    EXPECT_EQ(request.topic().SerializeAsString(), topic.SerializeAsString());
	EXPECT_NE(request.topic().SerializeAsString(), getWrongTopic().SerializeAsString());
    EXPECT_EQ(request.GetTypeName(), "uprotocol.core.usubscription.v3.NotificationsRequest");
}

}  // namespace uprotocol::core::usubscription::v3
