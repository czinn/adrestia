#include "push_challenges.h"

// Our related modules
#include "../adrestia_networking.h"
#include "../adrestia_database.h"

// Database
#include <pqxx/pqxx>

// System modules
#include <iostream>
#include <set>
using namespace std;
using namespace adrestia_networking;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

std::vector<json> adrestia_networking::PushChallenges::push(const Logger& logger, const string& uuid) {
  adrestia_database::Db db;
  auto query_result = db.query(R"sql(
    SELECT a.user_name, a.friend_code
    FROM challenges c
    JOIN adrestia_accounts a ON c.sender_uuid = a.uuid
    WHERE c.receiver_uuid = ?
  )sql")(uuid)();

  std::vector<json> result;
  for (const auto &row : query_result) {
    result.push_back({
      { HANDLER_KEY, "challenges" },
      { CODE_KEY, 200 },
      { MESSAGE_KEY, "New challenge" },
      { "user_name", row[0].as<string>() },
      { "friend_code", row[1].as<string>() },
    });
  }
  return result;
}
