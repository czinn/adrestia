/* Handling requests for 'abort_game'. */

// Us
#include "../adrestia_networking.h"

// Our related modules
#include "../adrestia_database.h"

// Database
#include <pqxx/pqxx>

// System modules
#include <iostream>
using namespace std;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

int adrestia_networking::handle_abort_game(const Logger& logger, const json& client_json, json& resp) {
  /* Immediately aborts the requested game.
   *
   * Accepts keys from client:
   *     HANDLER_KEY: <this function>
   *     "game_uid": The game uid to abort
   *
   * Keys inserted by babysitter:
   *     "uuid": The uuid of the client
   *
   * Responds to client with keys:
   *     HANDLER_KEY: <this function>
   *     CODE_KEY: 200 if deletion succeeded, 400 if game could not be found
   *     MESSAGE_KEY: "Game aborted." if abortion succeeded
   *                  "Bad game_uid." if game could not be found
   *
   * Should always return 0.
   */

  logger.trace("Triggered abort_game.");
  string uuid = client_json.at("uuid");
  string game_uid = client_json.at("game_uid");

  logger.debug("Attempting abort of game |%s| by user |%s|...",
               game_uid.c_str(), uuid.c_str()
              );
  try {
    pqxx::connection psql_connection = adrestia_database::establish_connection();
    adrestia_database::conclude_game_in_database(logger, psql_connection, game_uid, uuid, -1);
  }
  catch (const string& s) {
    logger.debug("Got error when attempting abort: |%s|.\n"
                 "Assuming this means that we couldn't find a matching uuid/game_uid...",
                 s.c_str()
                );
    resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
    resp[adrestia_networking::CODE_KEY] = 400;
    resp[adrestia_networking::MESSAGE_KEY] = "Bad game_uid.";
    return 0;
  }

  logger.info("Abort of game |%s| by user |%s| succeeded.",
              game_uid.c_str(), uuid.c_str()
             );

  resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
  resp[adrestia_networking::CODE_KEY] = 200;
  resp[adrestia_networking::MESSAGE_KEY] = "Game aborted.";
  return 0;
}