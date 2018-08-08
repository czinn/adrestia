#include "game_rules.h"

using namespace godot;

namespace godot {
  void GameRules::_register_methods() {
    REGISTER_JSONABLE(GameRules)
  }

  IMPL_JSONABLE(GameRules, _game_rules)
}
