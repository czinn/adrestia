/* Pushes active games to the client */

// Us
#include "push_active_games.h"

// Our related modules
#include "../adrestia_networking.h"
#include "../adrestia_database.h"

// Database
#include <pqxx/pqxx>

// System modules
#include <algorithm> // Find
#include <iostream>
#include <map>
#include <set>
using namespace std;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

adrestia_networking::PushActiveGames::PushActiveGames() {}

std::vector<json> adrestia_networking::PushActiveGames::push(const string& log_id, const string& uuid) {
  /* @brief Returns a list of messages for any new/changed games associated with the given uuid.
   *
   * @param log_id: A string prepended to diagnostics
   * @param uuid: The uuid for which games will be checked.
   *
   * @sideeffect: games_I_am_aware of will be updated to contain all things that have already been pushed to the
   *              client.
   * @sideeffect: active_game_uids_I_am_aware_of will be updated to reflect active games that have already been
   *              pushed to the client.
	 *
   * @returns: If there are new/changed games, the result will contain a message with the following keys:
   *             HANDLER_KEY: "push_active_games"
   *             CODE_KEY: 200
   *             MESSAGE_KEY: "You have [new|changed|new and changed|] games!" (depending on situation)
   *             "game_uids": A list of the new/changed game_uids
   *             "game_states": A list of the states of the new/changed game_uids. New games have no state
   *                            and are an empty string.
   *           Note that it follows that reported game_uids that do not represent concluded games are waiting
   *               for the player to make a move.
	 */

  // TODO: This pushes the whole gamestate! We must hide hidden information!

  // Check for games
  pqxx::connection* psql_connection = adrestia_database::establish_psql_connection();
  json active_games = adrestia_database::check_for_active_games_in_database(log_id, psql_connection, uuid);

  vector<string> active_game_uids = active_games["active_game_uids"];

  vector<string> game_uids_to_report;
  vector<json> game_states_to_report;
  bool new_games = false;
  bool changed_games = false;

	GameRules rules;

  // Any active games that have concluded require a special query.
  set<string> active_game_uids_set(active_game_uids.begin(), active_game_uids.end());
  for (unsigned int i = 0; i < active_game_uids_I_am_aware_of.size(); i += 1) {
    string current_game_uid = active_game_uids_I_am_aware_of[i];

    if (active_game_uids_set.find(current_game_uid) == active_game_uids_set.end()) {
      cout << "[" << log_id << "] Previously active game |" << current_game_uid << "| has become deactivated and should be reported." << endl;
      json current_game_state =
				adrestia_database::retrieve_gamestate_from_database(log_id, psql_connection, current_game_uid, rules);
      // A game_uid previously active has become deactivated. We should record this.
      game_uids_to_report.push_back(current_game_uid);
      game_states_to_report.push_back(current_game_state);
      changed_games = true;

      // We should add it to the map, and remove it from active games.
      games_I_am_aware_of[current_game_uid] = current_game_state;

      // The find should always succeed; if erase tries to erase active_game_uids_I_am_aware_of.end(),
      //     we want it to fail.
      active_game_uids_I_am_aware_of.erase(find(active_game_uids_I_am_aware_of.begin(),
                                                active_game_uids_I_am_aware_of.end(),
                                                current_game_state
                                               )
                                          );
    }
  }

  // Detect any new games, or games whose states have changed...
  for (unsigned int i = 0; i < active_game_uids.size(); i += 1) {
    string current_game_uid = active_game_uids[i];
    cout << "[" << log_id << "] Looking at current game with uid |" << current_game_uid << "|..." << endl;
		json current_game_state =
			adrestia_database::retrieve_gamestate_from_database(log_id, psql_connection, current_game_uid, rules);

    try {
      if (current_game_state != games_I_am_aware_of.at(current_game_uid)) {
        cout << "[" << log_id << "] Changed gamestate for game with game_uid |" << current_game_uid << "|" << endl;

        // The game state of this game has changed! We should record it.
        game_uids_to_report.push_back(current_game_uid);
        game_states_to_report.push_back(current_game_state);
        changed_games = true;

        // We should also add it to the map, seeing how it is becoming known.
        games_I_am_aware_of[current_game_uid] = current_game_state;
        active_game_uids_I_am_aware_of.push_back(current_game_uid);
      }
      else {
        cout << "[" << log_id << "] game_uid |" << current_game_uid << "| has no changes to its state." << endl;
      }
    }
    catch (out_of_range& oor) {
      cout << "[" << log_id << "] New active game with game_uid |" << current_game_uid << "|" << endl;

      // This game_uid was active, but it is not a known game uid.
      game_uids_to_report.push_back(current_game_uid);
      game_states_to_report.push_back(current_game_state);
      new_games = true;

      // We should also add it to the map, seeing how it is becoming known.
      games_I_am_aware_of[current_game_uid] = current_game_state;
      active_game_uids_I_am_aware_of.push_back(current_game_uid);
    }
  }

  delete psql_connection;

	std::vector<json> message_list;

  // If there is nothing to report, make an empty message and conclude.
  if (game_uids_to_report.empty()) {
    cout << "[" << log_id << "] No new/changed games for uuid |" << uuid << "|." << endl;
    return message_list;
  }

  // Decide what our message should be
  string api_message = "";
  if ((new_games) && (changed_games)) {
    api_message = "You have new and changed games!";
  }
  else if (new_games) {
    api_message = "You have new games!";
  }
  else {
    api_message = "You have changed games!";
  }
  cout << "[" << log_id << "] Message going in as: |" << api_message << "|" << endl;

  // Construct our json!
	json message_json;
  message_json[adrestia_networking::HANDLER_KEY] = "push_active_games";
  message_json[adrestia_networking::CODE_KEY] = 200;
  message_json[adrestia_networking::MESSAGE_KEY] = api_message;
  message_json["game_uids"] = game_uids_to_report;
  message_json["game_states"] = game_states_to_report;
	message_list.push_back(message_json);
  return message_list;
}
