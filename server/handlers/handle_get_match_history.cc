#include "../adrestia_networking.h"
#include "../adrestia_database.h"
#include "../logger.h"
#include "../../cpp/json.h"
using namespace std;
using json = nlohmann::json;

int adrestia_networking::handle_get_match_history(const Logger& logger, const json& client_json, json& resp) {
  logger.trace("Triggered handle_get_match_history.");
  adrestia_database::Db db;

  string uuid = client_json.at("uuid");

  auto result = db.query(R"sql(
    SELECT g.game_uid, g.game_state, g.creation_time, p1.player_id, g.winner_id, a.friend_code, a.user_name
    FROM adrestia_games g
    INNER JOIN adrestia_players p1 ON g.game_uid = p1.game_uid
    INNER JOIN adrestia_players p2 ON g.game_uid = p2.game_uid
    INNER JOIN adrestia_accounts a ON p2.user_uid = a.uuid
    WHERE p1.user_uid = ?
      AND p2.user_uid != ?
      AND g.activity_state != 0
      AND NOT EXISTS (SELECT r.id FROM adrestia_rules r WHERE r.id > g.game_rules_id)
    ORDER BY g.creation_time DESC
    LIMIT 10
  )sql")(uuid)(uuid)();

  vector<json> games;
  for (const auto &row : result) {
    games.push_back({
      {"game_uid", row["game_uid"].as<string>()},
      {"game_state", json::parse(row["game_state"].as<string>())},
      {"creation_time", row["creation_time"].as<string>()},
      {"player_id", row["player_id"].as<int>()},
      {"winner_id", row["winner_id"].as<int>()},
      {"opponent_user_name", row["user_name"].as<string>()},
      {"opponent_friend_code", row["friend_code"].as<string>()}
    });
  }

  resp[HANDLER_KEY] = client_json[HANDLER_KEY];
  resp_code(resp, 200, "Here are the games.");
  resp["games"] = games;

  return 0;
}
