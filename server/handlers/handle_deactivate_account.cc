/* Handling requests for 'deactivate_account'. */

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

int adrestia_networking::handle_deactivate_account(const Logger& logger, const json& client_json, json &resp) {
  /* Creates a new account in the database.
   *
   * Accepts keys from client:
   *     HANDLER_KEY: <this function>
   *
   * Keys inserted by babysitter:
   *     "uuid": The uuid of the client
   *
   * Responds to client with keys:
   *     HANDLER_KEY: <this function>
   *     CODE_KEY: 200
   *     MESSAGE_KEY: "Account deactivated."
   *
   * Always returns 0.
   */

  logger.trace("Triggered deactivate_account.");

  string uuid = client_json.at("uuid");

  adrestia_database::Db db(logger);
  db.query(R"sql(
    UPDATE adrestia_accounts
    SET deactivated = true
    WHERE uuid = ?
  )sql")(uuid)();
  db.commit();

  resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
  resp[adrestia_networking::CODE_KEY] = 200;
  resp[adrestia_networking::MESSAGE_KEY] = "Account deactivated.";

  logger.trace("deactivate_account concluded.");
  return 0;
}
