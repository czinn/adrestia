#pragma once

// Us
#include "../adrestia_networking.h"
#include "pusher.h"

// System modules
#include <set>
#include <vector>

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

namespace adrestia_networking {
	class PushChallenges : public Pusher {
		public:
			virtual std::vector<json> push(const Logger &logger, const std::string &uuid);
	};
}
