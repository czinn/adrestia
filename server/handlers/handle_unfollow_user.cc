#include "../adrestia_networking.h"
#include "../adrestia_database.h"
#include "../logger.h"
#include "../../cpp/json.h"
using namespace std;
using json = nlohmann::json;

int adrestia_networking::handle_unfollow_user(const Logger& logger, const json& client_json, json& resp) {
  logger.trace("Triggered handle_unfollow_user.");
  logger.error("Unimplemented");
  adrestia_database::Db db;
  return 1;
}
