#include "../adrestia_networking.h"
#include "../adrestia_database.h"
#include "../logger.h"
#include "../../cpp/json.h"
using namespace std;
using json = nlohmann::json;

int adrestia_networking::handle_send_challenge(const Logger& logger, const json& client_json, json& resp) {
  logger.trace("Triggered handle_send_challenge.");
  adrestia_database::Db db;

  string uuid = client_json.at("uuid");
  string friend_code = client_json.at("friend_code");

  db.query(R"sql(
    INSERT INTO challenges (sender_uuid, receiver_uuid)
    SELECT ?, uuid
    FROM adrestia_accounts
    WHERE friend_code = ?
  )sql")(uuid)(friend_code)();

  db.commit();

  resp[HANDLER_KEY] = client_json[HANDLER_KEY];
  resp[CODE_KEY] = 200;
  resp[MESSAGE_KEY] = "Done";
  return 0;
}
