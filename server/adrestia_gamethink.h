/* Header for serverside game processing and support functions */

#ifndef ADRESTIA_GAMETHINK_INCLUDE_GUARD
#define ADRESTIA_GAMETHINK_INCLUDE_GUARD

// Other adrestia modules
#include "../cpp/game_rules.h"

// System modules
#include <string>

// JSON
#include "../units_cpp/json.h"
using json = nlohmann::json;

namespace adrestia_gamethink {
  const std::string RULES_JSON_FILE("../common/rules.json");

  // Initializes GameRules object from file
  void initialize_game_rules(const std::string& log_id,
                             GameRules& game_rules
                            );
}

#endif
