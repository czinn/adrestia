// Us
#include "adrestia_gamethink.h"

// Our related modules
#include "../cpp/game_rules.h"

// System modules
#include <fstream>
#include <iostream>
using namespace std;

// JSON
#include "../units_cpp/json.h"
using json = nlohmann::json;


void adrestia_gamethink::initialize_game_rules(const string& log_id,
                                               GameRules& game_rules
                                              ) {
  /* @brief Initializes game_rules using from_json and the rules.json file,
   *        as specified in adrestia_gamethink.h.
   */

  ifstream in_stream(adrestia_gamethink::RULES_JSON_FILE);
  json file_as_json;
  in_stream >> file_as_json;
  from_json(file_as_json, game_rules);
}
