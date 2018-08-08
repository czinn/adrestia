#include "game_state.h"
#include "macros.h"
#include <game_rules.h>

using namespace godot;

namespace godot {
  void GameState::_register_methods() {
    register_method("init", &GameState::init);
    register_method("as_json", &GameState::as_json);
  }

  void GameState::init(godot::GameRules *rules, int num_players) {
    _game_state = std::make_shared<::GameState>(rules->_game_rules, num_players);
  }

  Variant GameState::as_json() {
    return to_godot_json(*_game_state);
  }
}
