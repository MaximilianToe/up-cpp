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

#include <google/protobuf/util/message_differencer.h>
#include <gtest/gtest.h>
#include <up-cpp/communication/NotificationSink.h>
#include <up-cpp/communication/NotificationSource.h>

#include "UTransportMock.h"
#include "up-cpp/datamodel/serializer/UUri.h"

namespace {
using namespace uprotocol::communication;
using namespace uprotocol::datamodel::serializer::uri;
using MsgDiff = google::protobuf::util::MessageDifferencer;

class NotificationTest : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}
	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	NotificationTest() = default;
	~NotificationTest() override= default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	[[nodiscard]] uprotocol::v1::UUri buildValidTestTopic() const;
	[[nodiscard]] uprotocol::v1::UUri buildValidDefaultSourceURI() const;
};

[[nodiscard]] uprotocol::v1::UUri NotificationTest::buildValidDefaultSourceURI()
    const {
	uprotocol::v1::UUri test_default_source_uri_;
	test_default_source_uri_.set_authority_name("10.0.0.1");
	test_default_source_uri_.set_ue_id(0x18000);
	test_default_source_uri_.set_ue_version_major(0x1);
	test_default_source_uri_.set_resource_id(0x0);
	return test_default_source_uri_;
}

[[nodiscard]] uprotocol::v1::UUri NotificationTest::buildValidTestTopic()
    const {
	uprotocol::v1::UUri test_topic_;
	test_topic_.set_authority_name("10.0.0.2");
	test_topic_.set_ue_id(0x00011101);
	test_topic_.set_ue_version_major(0xF8);
	test_topic_.set_resource_id(0x8101);
	return test_topic_;
}

TEST_F(NotificationTest, NotificationSuccess) {
	// Initialize
	uprotocol::v1::UPayloadFormat format =
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
	std::optional<uprotocol::v1::UPriority> priority =
	    uprotocol::v1::UPriority::UPRIORITY_CS1;

	std::optional<std::chrono::milliseconds> ttl =
	    std::chrono::milliseconds(1000);
	uprotocol::v1::UUri test_default_source_uri = buildValidDefaultSourceURI();
	uprotocol::v1::UUri test_topic = buildValidTestTopic();

	// Notify Sink
	auto transport_mock_notification_sink =
	    std::make_shared<uprotocol::test::UTransportMock>(test_default_source_uri);

	uprotocol::v1::UMessage capture_msg;
	auto callback = [&capture_msg](const auto& message) {
		capture_msg = message;
	};

	auto result = NotificationSink::create(transport_mock_notification_sink,
	                                       std::move(callback), test_topic);

	// Notify Source
	std::string test_payload_str = "test_payload";
	auto transport_mock_notification_source =
	    std::make_shared<uprotocol::test::UTransportMock>(test_default_source_uri);

	auto movable_topic = test_topic;

	NotificationSource notification_source(
	    transport_mock_notification_source, std::move(movable_topic),
	    std::move(test_default_source_uri), format, priority, ttl);

	uprotocol::datamodel::builder::Payload test_payload(test_payload_str, format);
	auto status = notification_source.notify(std::move(test_payload));

	EXPECT_EQ(
	    AsString::serialize(
	        transport_mock_notification_source->message_.attributes().source()),
	    AsString::serialize(transport_mock_notification_sink->source_filter_));

	EXPECT_EQ(
	    AsString::serialize(
	        transport_mock_notification_source->message_.attributes().sink()),
	    AsString::serialize(
	        transport_mock_notification_sink->sink_filter_.value()));

	// Manually bridge the two transports
	transport_mock_notification_sink->mockMessage(
	    transport_mock_notification_source->message_);

	// Test
	EXPECT_TRUE(MsgDiff::Equals(transport_mock_notification_source->message_,
	                            capture_msg));
	EXPECT_EQ(test_payload_str, capture_msg.payload());
}

}  // namespace
