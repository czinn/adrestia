/* Handling requests for 'authenticate'. */

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


int adrestia_networking::handle_authenticate(const Logger& _logger, const json& client_json, json& resp) {
  /* Checks if a given account/password combination exists in the database.
   *
   * Accepts keys from client:
   *     HANDLER_KEY: <this function>
   *     "uuid": The uuid to authenticate
   *     "password": The password
   *
   * Responds to client with keys:
   *     HANDLER_KEY: <this function>
   *     CODE_KEY: 200 if authentication succeeded; 401 otherwise.
   *     MESSAGE_KEY: (Message stating authorization succeeded, or did not, as necessary)
   *
   * Returns 0 if authentication succeeded, 1 otherwise.
   */

  logger.trace("Triggered authenticate.");
  string uuid = client_json.at("uuid");
  string password = client_json.at("password");

  logger.trace(
      "Checking authentication for account with:\n"
      "    uuid: |%s|\n"
      "    password: |%s|",
      uuid.c_str(),
      password.c_str());
  pqxx::connection psql_connection = adrestia_database::establish_connection();
  json result = adrestia_database::verify_existing_account_in_database(logger, psql_connection, uuid, password);

  resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];

  if (result.at("valid")) {
    // Update their last login time in the database
    adrestia_database::Db db(logger);
    db.query(R"sql(
      UPDATE adrestia_accounts
      SET last_login = NOW()
      WHERE uuid = ?
    )sql")(uuid)();
    db.commit();

    resp[adrestia_networking::CODE_KEY] = 200;
    resp[adrestia_networking::MESSAGE_KEY] = "Authorization OK.";
    resp["user_name"] = result.at("user_name").get<string>();
    resp["tag"] = result.at("tag").get<string>();
    return 0;
  } else {
    resp[adrestia_networking::CODE_KEY] = 401;
    resp[adrestia_networking::MESSAGE_KEY] = "Authorization NOT OK.";
    return 1;
  }

  return 1;
}
