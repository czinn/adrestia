#include "../adrestia_networking.h"
#include "../adrestia_database.h"
#include "../logger.h"
#include "../../cpp/json.h"
using namespace std;
using json = nlohmann::json;

int adrestia_networking::handle_follow_user(const Logger& logger, const json& client_json, json& resp) {
  logger.trace("Triggered handle_follow_user.");
  adrestia_database::Db db;

  string uuid = client_json.at("uuid");
  string friend_code = client_json.at("friend_code");

  auto result = db.query(R"sql(
    SELECT user_name, uuid
    FROM adrestia_accounts
    WHERE friend_code = ?
  )sql")(friend_code)();

  if (result.empty()) {
    resp[HANDLER_KEY] = client_json[HANDLER_KEY];
    resp_code(resp, 404, "Not found");
    return 0;
  }

  string uuid2 = result[0]["uuid"].as<string>();
  try {
    db.query(R"sql(
      INSERT INTO adrestia_follows (uuid1, uuid2)
      VALUES (?, ?)
    )sql")(uuid)(uuid2)();
    db.commit();

    resp[HANDLER_KEY] = client_json[HANDLER_KEY];
    resp_code(resp, 200, "Done");
    resp["user_name"] = result[0]["user_name"].as<string>();
    return 0;
  } catch (pqxx::integrity_constraint_violation &e) {
    db.abort();
  }

  resp[HANDLER_KEY] = client_json[HANDLER_KEY];
  resp_code(resp, 500, "Error"); // probably duplicate friend
  return 0;
}
