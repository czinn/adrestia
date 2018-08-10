#include "game_rules.h"
#include "macros.h"

using namespace godot;

namespace godot {
  void GameRules::_register_methods() {
    register_method("as_json", &GameRules::as_json);
  }

  Variant GameRules::as_json() {
    return to_godot_json(_game_rules);
  }
}
