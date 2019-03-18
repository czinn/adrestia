#include "../adrestia_networking.h"
#include "../adrestia_database.h"
#include "../logger.h"
#include "../../cpp/json.h"
using namespace std;
using json = nlohmann::json;

int adrestia_networking::handle_submit_single_player_game(const Logger& logger, const json& client_json, json& resp) {
  logger.trace("Triggered handle_submit_single_player_game.");
  adrestia_database::Db db;

  string uuid = client_json.at("uuid");
  Version version = string_to_version(client_json.at("rules_version"));
  json game_state = client_json.at("game_state");

  auto result = db.query(R"sql(
    INSERT INTO adrestia_single_player_games (user_uid, game_state, game_rules_id)
    SELECT ?, ?, id
    FROM adrestia_rules
    WHERE game_rules -> 'version' ->> 0 = ?
      AND game_rules -> 'version' ->> 1 = ?
      AND game_rules -> 'version' ->> 2 = ?
  )sql")(uuid)(game_state.dump())(version.major)(version.minor)(version.patch)();
  db.commit();

  resp[HANDLER_KEY] = client_json[HANDLER_KEY];
  resp_code(resp, 200, "Done");
  return 0;
}
