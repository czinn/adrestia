/* Pushes active games to the client */

// Us
#include "push_active_games.h"

// Our related modules
#include "../adrestia_networking.h"
#include "../adrestia_database.h"
#include "../../cpp/game_state.h"
#include "../../cpp/game_view.h"

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

std::vector<json> adrestia_networking::PushActiveGames::push(const Logger &logger, const string& uuid) {
  /* @brief Returns a list of messages for any new/changed games associated with the given uuid.
   *
   * @param logger: Logger.
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
   *             "updates": A list of updated game views or states, of the form
   *                        {"game_uid": uuid, "game_view": game view, "events": list of events}
	 *                        (or with "game_state" instead of "game_view" for
	 *                        completed games. If the game is a new game and has
	 *                        rules that aren't the most recent rules, the game's
	 *                        rules will be included in the first update.
   *           Note that it follows that reported game_views are waiting for
   *           the player to make a move.
   */

  // Check for games
  pqxx::connection psql_connection = adrestia_database::establish_connection();
  json active_games = adrestia_database::check_for_active_games_in_database(logger, psql_connection, uuid);

  vector<string> active_game_uids = active_games["active_game_uids"];

  vector<json> update_list;
  bool new_games = false;
  bool changed_games = false;

	GameRules latest_rules = adrestia_database::retrieve_game_rules(logger, psql_connection, 0);

  GameRules rules;

  // Any active games that have concluded require a special query.
  set<string> active_game_uids_set(active_game_uids.begin(), active_game_uids.end());
  for (const string &current_game_uid : active_game_uids_I_am_aware_of) {
    if (active_game_uids_set.find(current_game_uid) == active_game_uids_set.end()) {
      logger.info("Previously active game |%s| has become deactivated and should be reported.", current_game_uid.c_str());
      vector<json> events;
      json current_game_state =
        adrestia_database::retrieve_gamestate_from_database(
            logger, psql_connection, current_game_uid, rules, events);

      // A game_uid previously active has become deactivated. We should record this.
      update_list.push_back({
          {"game_uid", current_game_uid},
          {"game_state", current_game_state},
          {"events", events}});
      changed_games = true;

      // We should add it to the map, and remove it from active games.
      games_I_am_aware_of[current_game_uid] = current_game_state;
    }
  }

  // Detect any new games, or games whose states have changed...
  for (unsigned int i = 0; i < active_game_uids.size(); i += 1) {
    string current_game_uid = active_game_uids[i];
    logger.debug("Looking at current game with uid |%s|...", current_game_uid.c_str());
    vector<json> events;
    json current_game_state =
      adrestia_database::retrieve_gamestate_from_database(
          logger, psql_connection, current_game_uid, rules, events);

    json current_player_info =
      adrestia_database::retrieve_player_info_from_database(logger, psql_connection, current_game_uid, uuid);

    GameState game_state_obj(rules, current_game_state);
    GameView game_view_obj(game_state_obj, current_player_info["player_id"]);

    json current_game_view;
    to_json(current_game_view, game_view_obj);

    try {
      if (current_game_state != games_I_am_aware_of.at(current_game_uid)) {
        logger.info("Changed gamestate for game with game_uid |%s|", current_game_uid.c_str());

        // The game state of this game has changed! We should record it.
        update_list.push_back({
            {"game_uid", current_game_uid},
            {"game_view", current_game_view},
            {"events", events}});
        changed_games = true;

        // We should also add it to the map, seeing how it is becoming known.
        games_I_am_aware_of[current_game_uid] = current_game_state;
      }
      else {
        logger.debug("|%s| has no changes to its state.", current_game_uid.c_str());
      }
    }
    catch (out_of_range& oor) {
      logger.info("New active game with game_uid |%s|", current_game_uid.c_str());

      // This game_uid was active, but it is not a known game uid.
			json update = {
				{"game_uid", current_game_uid},
				{"game_view", current_game_view},
				{"events", events}
			};
			if (rules.get_version() != latest_rules.get_version()) {
				update["game_rules"] = rules;
			}
      update_list.push_back(update);
      new_games = true;

      // We should also add it to the map, seeing how it is becoming known.
      games_I_am_aware_of[current_game_uid] = current_game_state;
    }
  }

  active_game_uids_I_am_aware_of = active_game_uids_set;

  std::vector<json> message_list;

  // If there is nothing to report, make an empty message and conclude.
  if (update_list.empty()) {
    logger.debug("No new/changed games for uuid |%s|", uuid.c_str());
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
  logger.trace("Message going in as: |%s|", api_message.c_str());

  // Construct our json!
  json message_json;
  message_json[adrestia_networking::HANDLER_KEY] = "push_active_games";
  message_json[adrestia_networking::CODE_KEY] = 200;
  message_json[adrestia_networking::MESSAGE_KEY] = api_message;
  message_json["updates"] = update_list;
  message_list.push_back(message_json);
  return message_list;
}
