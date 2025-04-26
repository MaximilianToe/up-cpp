//
// Created by max on 23.04.25.
//

#include "up-cpp/core/usubscription/usubscription.h"

#include "up-cpp/communication/RpcServer.h"
#include "up-cpp/core/usubscription/handlers/subscribe.h"
#include "up-cpp/transport/UTransport.h"

namespace uprotocol::core::usubscription::v3 {

		using UTransport = uprotocol::transport::UTransport;
		using stopper_or_status = utils::Expected<USubscriptionStopper,v1::UStatus>;

		USubscriptionService::USubscriptionService(
            std::shared_ptr<UTransport> transport, USubscriptionConfig config,v1::UPayloadFormat format, std::chrono::milliseconds ttl)
            : RpcServer(std::move(transport), std::move(format), std::move(ttl)),
              config_(std::move(config)) {};

		stopper_or_status USubscriptionService::run() {
			v1::UUri subscribe_uuri(0x0001);
			v1::UStatus connection_status = connect(subscribe_uuri, handlers::subscribe);
			if(connection_status.code() != v1::UCode::OK) {
				return stopper_or_status(utils::Unexpected(connection_status));
			}

			return stopper_or_status(USubscriptionStopper());

	}

}// uprotocol::core::usubscription::v3
