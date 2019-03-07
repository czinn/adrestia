/* Handling requests for 'register_new_account'. */

// Us
#include "../adrestia_networking.h"

// Our related modules
#include "../adrestia_hexy.h"
#include "../adrestia_database.h"

// Database
#include <pqxx/pqxx>

// System modules
#include <iostream>
using namespace std;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;


int adrestia_networking::handle_register_new_account(const Logger& logger, const json& client_json, json &resp) {
  /* Creates a new account in the database.
   *
   * Accepts keys from client:
   *     HANDLER_KEY: <this function>
   *     "password" The password that the new account will have
   *     "debug" If present and true, the account is a debug account (exclude it from statistics)
   *
   * Responds to client with keys:
   *     HANDLER_KEY: <this function>
   *     CODE_KEY: 201
   *     MESSAGE_KEY: "Created new account."
   *     "uuid": The uuid of the new account
   *     "user_name": The user_name of the new account
   *     "tag": The tag of the new account
   *
   * Always returns 0.
   */

  logger.trace("Triggered register_new_account.");
  string password = client_json.at("password");

  const string default_user_name = "Guest";
  string salt = adrestia_hexy::hex_urandom(adrestia_database::SALT_LENGTH);
  pqxx::binarystring password_hash(adrestia_database::hash_password(password, salt));

  string uuid = adrestia_hexy::hex_urandom(adrestia_database::UUID_LENGTH);
  string tag = adrestia_hexy::hex_urandom(adrestia_database::TAG_LENGTH);
  bool debug = client_json.find("debug") != client_json.end() && client_json.at("debug").get<bool>();

  // Keep making up ids/tags until we get a successful insertion.
  for (int i = 0; i < 1000; i += 1) {
    string uuid = adrestia_hexy::hex_urandom(adrestia_database::UUID_LENGTH);
    string tag = adrestia_hexy::hex_urandom(adrestia_database::TAG_LENGTH);

    try {
      adrestia_database::Db db(logger);
      db.query(R"sql(
        INSERT INTO adrestia_accounts (uuid, user_name, tag, hash_of_salt_and_password, salt, last_login, debug)
        VALUES (?, ?, ?, ?, ?, NOW(), ?)
      )sql")(uuid)(default_user_name)(tag)(password_hash)(salt)(debug)();
      db.commit();

      logger.info_() << "Created new account with:" << endl
          << "    uuid: |" << uuid << "|" << endl
          << "    user_name: |" << default_user_name << "|" << endl
          << "    tag: |" << tag << "|" << endl;

      resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
      resp[adrestia_networking::CODE_KEY] = 201;
      resp[adrestia_networking::MESSAGE_KEY] = "Created new account.";
      resp["uuid"] = uuid;
      resp["user_name"] = default_user_name;
      resp["tag"] = tag;

      logger.trace("register_new_account concluded.");
      return 0;
    } catch (pqxx::integrity_constraint_violation &) {
      continue;
    }
  }

  logger.error("Failed to create a non-conflicting uuid/tag pair!");
  throw string("Failed to generate non-conflicting uuid/tag pair.");
}
