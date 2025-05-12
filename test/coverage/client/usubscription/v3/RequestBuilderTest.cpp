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

#include <gtest/gtest.h>
#include "up-cpp/client/usubscription/v3/RequestBuilder.h"
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <optional>
#include <chrono>
#include <google/protobuf/any.pb.h>

namespace uprotocol::core::usubscription::v3 {

class RequestBuilderTest : public ::testing::Test {
protected:
    RequestBuilder builder_;
    USubscriptionOptions options_;

    void SetUp() override {
        options_.permission_level = 1;
        options_.token = "sample_token";
        options_.when_expire = std::chrono::system_clock::now() + std::chrono::hours(1);
        options_.sample_period_ms = std::chrono::milliseconds(1000);
        options_.subscriber_details = google::protobuf::Any(); 
        options_.subscription_details = google::protobuf::Any();
        
        RequestBuilder builder(options_);
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
    v1::UUri topic;

    SubscriptionRequest request = builder_.buildSubscriptionRequest(topic);

    // Verify the attributes in the request
    // TODO(lennart) did not find appropriate member to check the remaining attributes
    EXPECT_EQ(request.topic().SerializeAsString(), topic.SerializeAsString());
    EXPECT_EQ(request.attributes().has_expire(), options_.when_expire.has_value());
    // EXPECT_EQ(request.attributes().subscription_details().SerializeAsString(), options_.subscription_details->SerializeAsString());
    EXPECT_EQ(request.attributes().sample_period_ms(), options_.sample_period_ms.value().count());
    // EXPECT_EQ(request.attributes().permission_level(), options_.permission_level.value());
    // EXPECT_EQ(request.attributes().token(), options_.token.value());
    // EXPECT_EQ(request.attributes().subscriber_details().SerializeAsString(), options_.subscriber_details->SerializeAsString());
}

} // namespace uprotocol::core::usubscription::v3
