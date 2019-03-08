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
    SELECT a.uuid, a.user_name, a.tag, a.last_login
    FROM adrestia_follows f
    JOIN adrestia_accounts a
    ON (a.uuid2 = f.uuid)
    WHERE uuid1 = ?
  )sql")(uuid)();

  vector<json> friends;
  for (const auto &row : result) {
    friends.push_back({
      {"uuid", row[0].as<string>()},
      {"user_name", row[1].as<string>()},
      {"last_login", row[2].as<string>()}
    });
  }

  resp["friends"] = friends;

  return 0;
}
