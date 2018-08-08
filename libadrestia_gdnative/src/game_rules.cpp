#include "game_rules.h"
#include "macros.h"

using namespace godot;

namespace godot {
  void GameRules::_register_methods() {
    register_method("load_json_string", &GameRules::load_json_string);
    register_method("as_json", &GameRules::as_json);
  }

  void GameRules::load_json_string(String str) {
    std::string s(str.ascii().get_data());
    _game_rules = nlohmann::json::parse(s);
  }

  Variant GameRules::as_json() {
    return to_godot_json(_game_rules);
  }
}
