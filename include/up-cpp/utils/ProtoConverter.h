#ifndef UP_CPP_UTILS_PROTOCONVERTER_H
#define UP_CPP_UTILS_PROTOCONVERTER_H

#include <google/protobuf/timestamp.pb.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <chrono>
#include <optional>
#include "up-cpp/datamodel/builder/Payload.h"
#include "up-cpp/utils/Expected.h"

namespace uprotocol::utils {
using uprotocol::core::usubscription::v3::SubscribeAttributes;
using uprotocol::core::usubscription::v3::SubscriberInfo;
using uprotocol::core::usubscription::v3::SubscriptionRequest;
using uprotocol::core::usubscription::v3::UnsubscribeRequest;

struct ProtoConverter {
	/// @brief Converts std::chrono::time_point to google::protobuf::Timestamp
	///
	/// @param tp the time point to convert
	/// @return the converted google::protobuf::Timestamp
	static google::protobuf::Timestamp ConvertToProtoTimestamp(
	    const std::chrono::system_clock::time_point& tp);

	/// @brief Builds a SubscriberInfo from the given parameters
	///
	/// @param entity_uri the UUri of the entity subscribing
	/// @return the built SubscriberInfo
	static SubscriberInfo BuildSubscriberInfo(const v1::UUri& entity_uri);

	/// @brief Builds a SubscribeAttributes from the given parameters
	///
	/// @param when_expire the optional time point when the subscription expires
	/// @param subscription_details the details of the subscription
	/// @param sample_period_ms the optional sample period in milliseconds
	/// @return the built SubscribeAttributes
	static SubscribeAttributes BuildSubscribeAttributes(
	    std::optional<std::chrono::system_clock::time_point> when_expire,
	    std::optional<google::protobuf::Any> subscription_details,
	    std::optional<std::chrono::milliseconds> sample_period_ms);

	/// @brief Builds a SubscriptionRequest from the given parameters
	///
	/// @param subscription_topic the UUri of the topic to subscribe to
	/// @param attributes the SubscribeAttributes for the subscription
	/// @return the built SubscriptionRequest
	static SubscriptionRequest BuildSubscriptionRequest(
	    const v1::UUri& subscription_topic,
	    std::optional<SubscribeAttributes> attributes = {});

	/// @brief Builds a UnsubscribeRequest from the given parameters
	///
	/// @param subscription_topic the UUri of the topic to unsubscribe from
	/// @return the built UnsubscribeRequest
	static UnsubscribeRequest BuildUnSubscribeRequest(
	    const v1::UUri& subscription_topic);
	static UnsubscribeRequest BuildUnSubscribeRequest(const v1::UUri& uri, const SubscribeAttributes& attributes);

	template <typename T>
	static utils::Expected<T, v1::UStatus> extractFromProtobuf(const v1::UMessage& message)
	{
		google::protobuf::Any any;

		if (!any.ParseFromString(message.payload())) {
			v1::UStatus status;
			status.set_code(v1::UCode::INTERNAL);
			status.set_message("extractFromProtobuf: Error when parsing payload.");
			return utils::Expected<T, v1::UStatus>(
				utils::Unexpected<v1::UStatus>(status));
		}

		T response;

		if (!any.UnpackTo(&response)) {
			v1::UStatus status;
			status.set_code(v1::UCode::INTERNAL);
			status.set_message("extractFromProtobuf: Error when unpacking any.");
			return utils::Expected<T, v1::UStatus>(
				utils::Unexpected<v1::UStatus>(status));
		}

		return Expected<T, v1::UStatus>(response);
	}
	
	template <typename T>
	static utils::Expected<datamodel::builder::Payload, v1::UStatus> protoToPayload(const T& proto) {
		google::protobuf::Any any;

		if (!any.PackFrom(proto)) {
			v1::UStatus status;
			status.set_code(v1::UCode::INTERNAL);
			status.set_message("protoToPayload: There was an error when serializing the subscription request.");
			return utils::Expected<datamodel::builder::Payload, v1::UStatus>(
				utils::Unexpected<v1::UStatus>(status));
		}

		const datamodel::builder::Payload payload(any);

		return utils::Expected<datamodel::builder::Payload, v1::UStatus>(payload);
	}

};
};      // namespace uprotocol::utils
#endif  // UP_CPP_UTILS_PROTOCONVERTER_H
