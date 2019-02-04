/* Handling requests for 'establish_connection'. */

// Us
#include "../adrestia_database.h"
#include "../adrestia_networking.h"
#include "../versioning.h"
#include "../../cpp/game_rules.h"

// Database
#include <pqxx/pqxx>

// System modules
#include <iostream>
using namespace std;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

int adrestia_networking::handle_establish_connection(const string& log_id, const json& client_json, json& resp) {
  /* Responds with a message.
   *
   * Accepts keys from client:
   *     HANDLER_KEY: <this function>
   *     "client_version": String of the form "X.Y.Z" where X, Y, and Z are numbers.
   *
   * Responds to client with keys:
   *     HANDLER_KEY: <this function>
   *     CODE_KEY: 200
   *     MESSAGE_KEY: "Adrestia is listening."
   *     "game_rules": The most recent set of game rules.
   *
   * Returns 0 if the connection was successfully established.
   * Returns 1 if the connection failed for some reason (out-of-date client).
   */
  Version v = string_to_version(client_json.at("client_version"));
  if (v.major < adrestia_networking::CLIENT_VERSION.major) {
    // Client is too old. Reject connection.
    resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
    resp[adrestia_networking::CODE_KEY] = 400;
    resp[adrestia_networking::MESSAGE_KEY] = "Game client too old. Update the app for multiplayer.";
    return 1;
  }
  if (v > adrestia_networking::CLIENT_VERSION) {
    // Client is too new. Reject connection.
    resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
    resp[adrestia_networking::CODE_KEY] = 400;
    resp[adrestia_networking::MESSAGE_KEY] = "Server out of date, please try again later";
    cout << "CRITICAL WARNING: Client connected with version " << version_to_string(v)
         << ", which is newer than the newest client. Update the server?" << std::endl;
    return 1;
  }

  pqxx::connection* psql_connection = adrestia_database::establish_psql_connection();
  GameRules game_rules = adrestia_database::retrieve_game_rules(log_id, psql_connection, 0);

  resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
  resp[adrestia_networking::CODE_KEY] = 200;
  resp[adrestia_networking::MESSAGE_KEY] = "Adrestia is listening.";
  resp["game_rules"] = json(game_rules);

  return 0;
}
