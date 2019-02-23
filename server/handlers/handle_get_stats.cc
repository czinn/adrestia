/* Handling requests for 'get_stats'. */

// Us
#include "../adrestia_networking.h"

// Our related modules
#include "../adrestia_database.h"
#include "../adrestia_hexy.h"

// Database
#include <pqxx/pqxx>

// System modules
#include <iostream>
using namespace std;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

int adrestia_networking::handle_get_stats(const Logger& logger, const json& client_json, json& resp) {
  /* Gets the stats for the given user.
   *
   * Accepts keys from client:
   *     HANDLER_KEY: <this function>
   *
   * Keys inserted by babysitter:
   *     "uuid": The uuid for the user
   *
   * Responds to client with keys:
   *     HANDLER_KEY: <this function>
   *     CODE_KEY: 200
   *     MESSAGE_KEY: "Here are your stats"
   *     "wins": The number of wins the player has
   *     "games": The number of games the player has played
   *
   * Always returns 0.
   */

  string uuid = client_json.at("uuid");
	logger.trace_() << "Getting stats for uuid |" << uuid << "|" << endl;

  adrestia_database::Db db(logger);
	auto wins_result = db.query(R"sql(
		SELECT COUNT(*)
		FROM adrestia_games
			INNER JOIN adrestia_players ON adrestia_games.game_uid = adrestia_players.game_uid
		WHERE user_uid = ?
			AND winner_id = player_id
		)sql")(uuid)();
	int wins = wins_result[0][0].as<int>();

	auto games_result = db.query(R"sql(
		SELECT COUNT(*)
		FROM adrestia_games
			INNER JOIN adrestia_players ON adrestia_games.game_uid = adrestia_players.game_uid
		WHERE user_uid = ?
		)sql")(uuid)();
	int games = games_result[0][0].as<int>();

	logger.trace_() << "User |" << uuid << "| has " << wins << " wins out of " << games << " games" << endl;

  resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
  resp[adrestia_networking::CODE_KEY] = 200;
  resp[adrestia_networking::MESSAGE_KEY] = "Here are your stats";
  resp["wins"] = wins;
  resp["games"] = games;
	
	logger.trace("get_stats concluded.");
	return 0;
}
