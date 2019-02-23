/* Handling requests for 'do_turn'. */

// Us
#include "../adrestia_networking.h"

// Our related modules
#include "../adrestia_database.h"
#include "../../cpp/game_rules.h"
#include "../../cpp/game_state.h"

// Database
#include <pqxx/pqxx>

// System modules
using namespace std;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

int adrestia_networking::handle_submit_move(const Logger& logger, const json& client_json, json& resp) {
  /* Submits the move for a turn in the game, updating the game if all players
   * have submitted a move.
   *
   * Accepts keys from client:
   *      HANDLER_KEY: <this function>
   *      "game_uid": The ID of the game to submit the move for
   *      "player_move": A list of spell IDs
   *
   * Keys inserted by babysitter:
   *     "uuid": The uuid of the client
   *
   * Responds to client with keys:
   *     HANDLER_KEY: <this function>
   *     CODE_KEY: 200 if successful, 400 is illegal move
   *     MESSAGE_KEY: "Legal move" or "Illegal move"
   *
   * Should always return 0.
   */

  logger.trace("Triggered submit_move");
  string uuid = client_json.at("uuid");
  string game_uid = client_json.at("game_uid");
  vector<string> player_move = client_json.at("player_move");
  logger.trace("Player |%s| submitted move |%s| to game |%s|", uuid.c_str(),
      client_json.at("player_move").dump().c_str(), game_uid.c_str());

  pqxx::connection conn = adrestia_database::establish_connection();
  bool legal_move =
    adrestia_database::submit_move_in_database(
        logger, conn, uuid, game_uid, player_move);

  resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
  resp[adrestia_networking::CODE_KEY] = legal_move ? 200 : 400;
  resp[adrestia_networking::MESSAGE_KEY] = legal_move ? "Legal move" : "Illegal move";
  return 0;
}
