#include <gtest/gtest.h>

#include "UTransportMock.h"
#include "up-cpp/client/usubscription/v3/RequestBuilder.h"
#include "up-cpp/client/usubscription/v3/RpcClientUSubscription.h"
#include "up-cpp/communication/RpcServer.h"
#include "up-cpp/utils/ProtoConverter.h"
#include "uprotocol/v1/uri.pb.h"

using UMessage = uprotocol::v1::UMessage;
using Payload = uprotocol::datamodel::builder::Payload;
using ProtoConverter = uprotocol::utils::ProtoConverter;

namespace {

constexpr uint32_t UE_VERSION_MAJOR = 3;
constexpr uint32_t CLIENT_UE_ID = 23492;

class RpcClientUSubscriptionTest : public testing::Test {
protected:
	// Run once per TEST_F.s
	// Used to set up clean environments per test.
	void SetUp() override {
		uprotocol::v1::UUri client_uuri;
		client_uuri.set_authority_name("client.usubscription");
		client_uuri.set_ue_id(CLIENT_UE_ID);
		client_uuri.set_ue_version_major(UE_VERSION_MAJOR);
		client_uuri.set_resource_id(0);

		client_transport_ =
		    std::make_shared<uprotocol::test::UTransportMock>(client_uuri);

		uprotocol::v1::UUri server_uuri;
		server_uuri.set_authority_name("core.usubscription");
		server_uuri.set_ue_id(1);
		server_uuri.set_ue_version_major(UE_VERSION_MAJOR);
		server_uuri.set_resource_id(0);

		server_transport_ =
		    std::make_shared<uprotocol::test::UTransportMock>(server_uuri);

		constexpr uint32_t SERVER_RESOURCE_ID = 32600;
		server_method_uuri_.set_authority_name("core.usubscription");
		server_method_uuri_.set_ue_id(1);
		server_method_uuri_.set_ue_version_major(UE_VERSION_MAJOR);
		server_method_uuri_.set_resource_id(SERVER_RESOURCE_ID);

		constexpr uint32_t TOPIC_UE = 2342;
		constexpr uint32_t TOPIC_RESOURCE_ID = 12340;
		subscription_topic_.set_authority_name("topic.usubscription");
		subscription_topic_.set_ue_id(TOPIC_UE);
		subscription_topic_.set_ue_version_major(UE_VERSION_MAJOR);
		subscription_topic_.set_resource_id(TOPIC_RESOURCE_ID);
	}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	RpcClientUSubscriptionTest() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	std::shared_ptr<uprotocol::test::UTransportMock> getClientTransport() {
		return client_transport_;
	}

	std::shared_ptr<uprotocol::test::UTransportMock> getServerTransport() {
		return server_transport_;
	}

	uprotocol::v1::UUri getServerMethodUuri() { return server_method_uuri_; }

	uprotocol::v1::UUri getSubscriptionTopic() { return subscription_topic_; }

	uprotocol::core::usubscription::v3::RequestBuilder getRequestBuilder() {
		return request_builder_;
	}

private:
	std::shared_ptr<uprotocol::test::UTransportMock> client_transport_;
	std::shared_ptr<uprotocol::test::UTransportMock> server_transport_;
	uprotocol::core::usubscription::v3::RequestBuilder request_builder_;
	uprotocol::v1::UUri server_method_uuri_;
	uprotocol::v1::UUri subscription_topic_;

public:
	~RpcClientUSubscriptionTest() override = default;
};

TEST_F(RpcClientUSubscriptionTest, SubscribeRoundtripWithValidProtoPayload) {
	bool server_callback_executed = false;
	uprotocol::core::usubscription::v3::SubscriptionRequest server_capture;
	uprotocol::core::usubscription::v3::SubscriptionResponse server_response;
	*server_response.mutable_topic() = getSubscriptionTopic();
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status = ProtoConverter::extractFromProtobuf<
		        uprotocol::core::usubscription::v3::SubscriptionRequest>(
		        message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    Payload response_payload(server_response);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto subscription_request =
	    getRequestBuilder().buildSubscriptionRequest(getSubscriptionTopic());

	auto response_or_status_future = std::async(
	    std::launch::async,
	    [&client, &subscription_request]()
	        -> uprotocol::utils::Expected<
	            uprotocol::core::usubscription::v3::SubscriptionResponse,
	            uprotocol::v1::UStatus> {
		    return client.subscribe(subscription_request);
	    });

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	constexpr int ITERATIONS_TILL_TIMEOUT = 10;
	constexpr std::chrono::milliseconds MILLISECONDS_PER_ITERATION =
	    std::chrono::milliseconds(100);
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          subscription_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

TEST_F(RpcClientUSubscriptionTest, SubscribeRoundtripWithValidProtoAnyPayload) {
	bool server_callback_executed = false;
	uprotocol::core::usubscription::v3::SubscriptionRequest server_capture;
	uprotocol::core::usubscription::v3::SubscriptionResponse server_response;
	*server_response.mutable_topic() = getSubscriptionTopic();
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status = ProtoConverter::extractFromProtobuf<
		        uprotocol::core::usubscription::v3::SubscriptionRequest>(
		        message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
			google::protobuf::Any any;
			if(!any.PackFrom(server_response)) {
				return std::nullopt;
			}
			Payload response_payload(any);
			return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto subscription_request =
	    getRequestBuilder().buildSubscriptionRequest(getSubscriptionTopic());

	auto response_or_status_future = std::async(
	    std::launch::async,
	    [&client, &subscription_request]()
	        -> uprotocol::utils::Expected<
	            uprotocol::core::usubscription::v3::SubscriptionResponse,
	            uprotocol::v1::UStatus> {
		    return client.subscribe(subscription_request);
	    });

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	constexpr int ITERATIONS_TILL_TIMEOUT = 10;
	constexpr std::chrono::milliseconds MILLISECONDS_PER_ITERATION =
	    std::chrono::milliseconds(100);
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          subscription_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}

TEST_F(RpcClientUSubscriptionTest, SubscribeRoundtripWithInvalidPayload) {
	bool server_callback_executed = false;
	uprotocol::core::usubscription::v3::SubscriptionRequest server_capture;
	uprotocol::core::usubscription::v3::SubscriptionResponse server_response;
	*server_response.mutable_topic() = getSubscriptionTopic();
	auto server_or_status = uprotocol::communication::RpcServer::create(
	    getServerTransport(), getServerMethodUuri(),
	    [&server_callback_executed, &server_capture,
	     &server_response](const UMessage& message) -> std::optional<Payload> {
		    server_callback_executed = true;
		    auto request_or_status = ProtoConverter::extractFromProtobuf<
		        uprotocol::core::usubscription::v3::SubscriptionRequest>(
		        message);
		    if (!request_or_status.has_value()) {
			    return std::nullopt;
		    }
		    server_capture = request_or_status.value();
		    Payload response_payload(server_response);
		    return response_payload;
	    },
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);

	ASSERT_TRUE(server_or_status.has_value());
	ASSERT_NE(server_or_status.value(), nullptr);
	EXPECT_TRUE(getServerTransport()->getListener());

	auto client = uprotocol::core::usubscription::v3::RpcClientUSubscription(
	    getClientTransport());

	const auto subscription_request =
	    getRequestBuilder().buildSubscriptionRequest(getSubscriptionTopic());

	auto response_or_status_future = std::async(
	    std::launch::async,
	    [&client, &subscription_request]()
	        -> uprotocol::utils::Expected<
	            uprotocol::core::usubscription::v3::SubscriptionResponse,
	            uprotocol::v1::UStatus> {
		    return client.subscribe(subscription_request);
	    });

	// wait to give the client time to send the request. Otherwise this would
	// cause a race condition
	constexpr int ITERATIONS_TILL_TIMEOUT = 10;
	constexpr std::chrono::milliseconds MILLISECONDS_PER_ITERATION =
	    std::chrono::milliseconds(100);
	int counter = ITERATIONS_TILL_TIMEOUT;
	while (counter > 0 && getClientTransport()->getSendCount() == 0) {
		counter--;
		std::this_thread::sleep_for(MILLISECONDS_PER_ITERATION);
	}
	ASSERT_EQ(getClientTransport()->getSendCount(), 1);
	EXPECT_TRUE(getClientTransport()->getListener());

	(*getServerTransport()->getListener())(getClientTransport()->getMessage());
	EXPECT_TRUE(server_callback_executed);
	EXPECT_EQ(server_capture.SerializeAsString(),
	          subscription_request.SerializeAsString());

	getClientTransport()->mockMessage(getServerTransport()->getMessage());
	EXPECT_TRUE(getClientTransport()->getListener());
	EXPECT_EQ(getClientTransport()->getSendCount(), 1);
	auto response_or_status = response_or_status_future.get();
	ASSERT_TRUE(response_or_status.has_value());
	EXPECT_EQ(response_or_status.value().SerializeAsString(),
	          server_response.SerializeAsString());
}
TEST_F(RpcClientUSubscriptionTest, SubscribeRoundtripWithInvalidResponse) {}
};  // namespace
