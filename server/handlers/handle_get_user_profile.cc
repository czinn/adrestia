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

  auto result = db.query(R"sql(
    SELECT user_name, tag, last_login
    FROM adrestia_accounts
    WHERE uuid = ?
  )sql")(uuid)();

  if (result.size() == 0) {
    resp_code(resp, 404, "Not found");
    resp[HANDLER_KEY] = client_json[HANDLER_KEY];
    return 0;
  }

  auto row = result[0];
  resp[HANDLER_KEY] = client_json[HANDLER_KEY];
  resp_code(resp, 200, "Here is the profile.");
  resp["user_name"] = row[0].as<string>();
  resp["tag"] = row[1].as<string>();
  resp["last_login"] = row[2].as<string>();
  return 0;
}
