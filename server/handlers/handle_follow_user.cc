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
  string their_uuid = client_json.at("their_uuid");

  db.query(R"sql(
    INSERT INTO adrestia_follows (uuid1, uuid2)
    VALUES (?, ?)
  )sql")(uuid)(their_uuid)();

  return 0;
}
