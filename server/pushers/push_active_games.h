#pragma once
/* Pushes active games to the client */

// Us
#include "../adrestia_networking.h"
#include "pusher.h"

// System modules
#include <map>
#include <string>
#include <vector>

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

namespace adrestia_networking {
	class PushActiveGames : public Pusher {
		public:
			PushActiveGames();
			virtual std::vector<json> push(const Logger &logger, const std::string &uuid);
		
		private:
			std::map<std::string, json> games_I_am_aware_of; // game_uid to game_state
			std::vector<std::string> active_game_uids_I_am_aware_of;
	};
}
