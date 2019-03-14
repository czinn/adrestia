#include "../adrestia_networking.h"
#include "../adrestia_database.h"
#include <pqxx/pqxx>
#include <iostream>
#include "../../cpp/json.h"
using namespace std;
using namespace adrestia_networking;
using json = nlohmann::json;

int adrestia_networking::handle_get_user_profile(const Logger& logger, const json& client_json, json& resp) {
  logger.trace("Triggered handle_get_user_profile.");
  adrestia_database::Db db;
  string uuid = client_json.at("uuid");
  string friend_code = client_json.at("friend_code");

  auto result = db.query(R"sql(
    SELECT uuid, user_name, friend_code, last_login, is_online
    FROM adrestia_accounts
    WHERE friend_code = ?
  )sql")(friend_code)();

  if (result.size() == 0) {
    resp_code(resp, 404, "Not found");
    resp[HANDLER_KEY] = client_json[HANDLER_KEY];
    return 0;
  }

  resp[HANDLER_KEY] = client_json[HANDLER_KEY];
  resp_code(resp, 200, "Here is the profile.");
  json profile;
  auto row = result[0];
  profile["uuid"] = row["uuid"].as<string>();
  profile["user_name"] = row["user_name"].as<string>();
  profile["friend_code"] = row["friend_code"].as<string>();
  profile["last_login"] = row["last_login"].as<string>();
  profile["is_online"] = row["is_online"].as<bool>();
  resp["profile"] = profile;
  return 0;
}
