#pragma once

// System modules
#include <string>
#include <vector>

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

#include "../logger.h"

class Pusher {
	public:
		/* @brief Returns a list of messages that should be sent to the client,
		 * which may be empty.
		 *
		 * @param logger: Logger
		 * @param uuid: The uuid for the player to whom the messages will be sent
		 *
		 * @returns A list of messages to push.
		 */
		virtual std::vector<json> push(const Logger &logger, const std::string &uuid) = 0;
};
