#include "../adrestia_networking.h"
#include "../adrestia_database.h"
#include "../logger.h"
#include "../../cpp/json.h"
using namespace std;
using json = nlohmann::json;

int adrestia_networking::handle_unfollow_user(const Logger& logger, const json& client_json, json& resp) {
  logger.trace("Triggered handle_unfollow_user.");
  adrestia_database::Db db;

  string uuid = client_json.at("uuid");
  string friend_code = client_json.at("friend_code");

  auto result = db.query(R"sql(
    SELECT uuid
    FROM adrestia_accounts
    WHERE friend_code = ?
  )sql")(friend_code)();

  if (result.empty()) {
    resp[HANDLER_KEY] = client_json[HANDLER_KEY];
    resp_code(resp, 404, "Not found");
    return 0;
  }

  string uuid2 = result[0]["friend_code"].as<string>();
  db.query(R"sql(
    DELETE FROM adrestia_follows (uuid1, uuid2)
    WHERE uuid1 = ? AND uuid2 = ?
  )sql")(uuid)(uuid2)();

  resp[HANDLER_KEY] = client_json[HANDLER_KEY];
  resp_code(resp, 200, "Done");
  return 0;
}
