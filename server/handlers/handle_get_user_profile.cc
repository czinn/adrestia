#include "../adrestia_networking.h"
#include "../adrestia_database.h"
#include <pqxx/pqxx>
#include <iostream>
#include "../../cpp/json.h"
using namespace std;
using json = nlohmann::json;

int adrestia_networking::handle_get_user_profile(const Logger& logger, const json& client_json, json& resp) {
  logger.trace("Triggered handle_get_user_profile.");
  adrestia_database::Db db;
  return 1;
}
