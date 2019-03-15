/* Handling requests for 'change_user_name'. */

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


int adrestia_networking::handle_change_user_name(const Logger& logger, const json& client_json, json& resp) {
  /* Changes the user name associated with the given uuid to the new requested user name.
   *     By necessity, creates a new tag to go with this user_name.
   *
   * Accepts keys from client:
   *     HANDLER_KEY: <this function>
   *     "user_name": The new user_name
   *
   * Keys inserted by babysitter:
   *     "uuid": The uuid to modify
   *
   * Responds to client with keys:
   *     HANDLER_KEY: <this function>
   *     CODE_KEY: 200
   *     MESSAGE_KEY: "Modification complete."
   *     "tag": The new tag.
   *     "user_name": The new user_name.
   *
   * Always returns 0.
   */

  string uuid = client_json.at("uuid");
  string new_user_name = client_json.at("user_name");
  logger.trace_()
    << "Triggered change_user_name with args:\n"
    << "    uuid: |" << uuid << "|\n"
    << "    user_name: |" << new_user_name << "|" << endl;

  logger.trace("Modifying uuid |%s| to have user_name |%s|...", uuid.c_str(), new_user_name.c_str());

  adrestia_database::Db db(logger);
  for (int i = 0; i < 1000; i += 1) {
    string tag = adrestia_hexy::hex_urandom(adrestia_database::TAG_LENGTH);
    string friend_code = adrestia_hexy::random_dec_string(adrestia_database::FRIEND_CODE_LENGTH);
    try {
      pqxx::result result = db.query(R"sql(
        UPDATE adrestia_accounts
        SET user_name = ?, tag = ?, friend_code = ?
        WHERE uuid = ?
      )sql")(new_user_name)(tag)(friend_code)(uuid)();
      db.commit();
      logger.info("Successfully changed user_name in database.");

      logger.trace_()
        << "New account info is:" << endl
        << "    uuid: |" << uuid << "|" << endl
        << "    user_name: |" << new_user_name << "|" << endl
        << "    tag: |" << tag << "|" << endl;

      resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
      resp[adrestia_networking::CODE_KEY] = 200;
      resp[adrestia_networking::MESSAGE_KEY] = "Modification complete.";
      resp["tag"] = tag;
      resp["user_name"] = new_user_name;
      resp["friend_code"] = friend_code;

      logger.trace("change_user_name concluded.");
      return 0;
    } catch (pqxx::integrity_constraint_violation &e) {
      db.abort();
    }
  }

  logger.error("Failed to update the user_name!");
  throw string("Failed to update user name of uuid |" + uuid + "| to user_name |" + new_user_name + "|!");
}
