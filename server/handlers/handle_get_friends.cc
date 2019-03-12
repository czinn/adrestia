#include "../adrestia_networking.h"
#include "../adrestia_database.h"
#include "../logger.h"
#include "../../cpp/json.h"
using namespace std;
using json = nlohmann::json;

int adrestia_networking::handle_get_friends(const Logger& logger, const json& client_json, json& resp) {
  logger.trace("Triggered handle_get_friends.");
  adrestia_database::Db db;

  string uuid = client_json.at("uuid");

  auto result = db.query(R"sql(
    SELECT a.uuid, a.user_name, a.tag, a.last_login, a.is_online
    FROM adrestia_follows f
    JOIN adrestia_accounts a
    ON (f.uuid2 = a.uuid)
    WHERE uuid1 = ?
  )sql")(uuid)();

  vector<json> friends;
  for (const auto &row : result) {
    friends.push_back({
      {"uuid", row["uuid"].as<string>()},
      {"user_name", row["user_name"].as<string>()},
      {"tag", row["tag"].as<string>()},
      {"last_login", row["last_login"].as<string>()},
      {"is_online", row["is_online"].as<string>()}
    });
  }

  resp[HANDLER_KEY] = client_json[HANDLER_KEY];
  resp_code(resp, 200, "Here are the friends.");
  resp["friends"] = friends;

  return 0;
}
